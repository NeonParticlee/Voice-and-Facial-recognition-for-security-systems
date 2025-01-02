#ifndef CNN_H
#define CNN_H
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cudnn.h"
#include <stdio.h>
#include <stdlib.h>

/*
steps to implement this CNN:
1 - load our images using stb (ideally walking through a directory of multiple images would be great)
2 - allocate the pixel buffers and move to GPU
3 - allocate our weights and initialize them
4 - pass the pixel data with the classification label (person's name) - pass in batches or 1 image at a time, batches are better since we're multithreading
5 - apply convolution(important feature extraction), pooling(feature compressions) each layer
6 - backpropagation and ADAM for optimizing the weights of the model
7 - finally, pass through softmax to compute probabilities of the labels and look through the
file database and match the computed prediction with existing training data
8 - if the model cannot find the label classification it had computed, it should store the label as a new face in the model's database
*/

#define CHECK_CUDNN(cudnnFuncCall)                                      \
{   \
cudnnStatus_t err = cudnnFuncCall;                                                \
    if (err != CUDNN_STATUS_SUCCESS)                                \
    {                                                               \
        fprintf(stderr, "error in cudnn status check: %s\n", cudnnGetErrorString(err));           \
        exit(EXIT_FAILURE);                                                  \
    }                                                               \
}



typedef struct
{
	cudnnTensorDescriptor_t inputDesc, outputDesc, biasDesc;
	cudnnFilterDescriptor_t filterDesc;
	cudnnConvolutionDescriptor_t convDesc;
	/*LAYER MEMORY IN THR GPU*/
	float* weights;
	float* gradWeights;/*gradients of the weight values, used to subtract the weights each update, also one for biases*/

	float* biases;
	float* gradBiases;

	float* outputs;

} ConvolutionLayer;

void GPU_malloc(float** d_data, size_t size)
{
	cudaError_t err = cudaMalloc(d_data, size);
	if (err != cudaSuccess)
	{
		fprintf(stderr, "error in allocating device pointer: %s", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

void initConvolutionLayer(ConvolutionLayer* layer, int inputChannels, int outputChannels,
	int kernelSize, int inputWidth, int inputHeight)
{
	/*initialize the layer's descriptors*/
	CHECK_CUDNN(cudnnCreateTensorDescriptor(&layer->inputDesc));
	CHECK_CUDNN(cudnnCreateTensorDescriptor(&layer->outputDesc));
	CHECK_CUDNN(cudnnCreateTensorDescriptor(&layer->biasDesc));
	CHECK_CUDNN(cudnnCreateFilterDescriptor(&layer->filterDesc));
	CHECK_CUDNN(cudnnCreateConvolutionDescriptor(&layer->convDesc));

	//input and output tensor descriptors
	CHECK_CUDNN(cudnnSetTensor4dDescriptor(layer->inputDesc, CUDNN_TENSOR_NCHW,
		CUDNN_DATA_FLOAT, 1, inputChannels, inputHeight, inputWidth));

	CHECK_CUDNN(cudnnSetTensor4dDescriptor(layer->outputDesc, CUDNN_TENSOR_NCHW,
		CUDNN_DATA_FLOAT, 1, outputChannels, inputHeight, inputWidth));

	CHECK_CUDNN(cudnnSetTensor4dDescriptor(layer->biasDesc, CUDNN_TENSOR_NCHW, CUDNN_DATA_FLOAT, 1, outputChannels, 1, 1));

	//filter descriptor
	CHECK_CUDNN(cudnnSetFilter4dDescriptor(layer->filterDesc, CUDNN_DATA_FLOAT,
		CUDNN_TENSOR_NCHW, outputChannels, inputChannels, kernelSize, kernelSize));
	//convolution descriptor
	CHECK_CUDNN(cudnnSetConvolution2dDescriptor(layer->convDesc, 1, 1, 1, 1, 1, 1,
		CUDNN_CONVOLUTION, CUDNN_DATA_FLOAT));

	//allocating GPU memory for weights and biases (weights take up the biggest size (since they are corresponding to all pixel data)
	GPU_malloc(&layer->weights, outputChannels * inputChannels * kernelSize * kernelSize * sizeof(float));
	GPU_malloc(&layer->biases, outputChannels * sizeof(float));
	GPU_malloc(&layer->outputs, 1 * outputChannels * inputHeight * inputWidth * sizeof(float));
}

void forwardConvolutionLayer(cudnnHandle_t cudnn, ConvolutionLayer* layer, float* inputData)
{
	const float alpha = 1.0f, beta = 0.0f;
	cudnnConvolutionForward(cudnn, &alpha, layer->inputDesc, inputData,
		layer->filterDesc, layer->weights,
		layer->convDesc, CUDNN_CONVOLUTION_FWD_ALGO_IMPLICIT_GEMM, /*using the optimized general matrix multiplication algorithm (GEMM) in forward propagation*/
		NULL, 0, &beta, layer->outputDesc, layer->outputs);

	CHECK_CUDNN(cudnnAddTensor(cudnn, &alpha, layer->biasDesc, layer->biases,
		&beta, layer->outputDesc, layer->outputs));
}

void backPropConvolutionLayer(cudnnHandle_t cudnn, ConvolutionLayer* layer, float* inputData, float* gradients)
{
	const float alpha = 1.0f, beta = 0.0f;
	//compute d/dw b
	CHECK_CUDNN(cudnnConvolutionBackwardFilter(cudnn, &alpha, layer->inputDesc,
		inputData, layer->outputDesc, gradients, layer->convDesc, CUDNN_CONVOLUTION_BWD_FILTER_ALGO_1,
		NULL, 0, &beta, layer->filterDesc, layer->gradWeights));
	//compute d/db
	CHECK_CUDNN(cudnnConvolutionBackwardBias(cudnn, &alpha, layer->outputDesc,
		gradients, &beta, layer->biasDesc, layer->gradBiases));
}

__global__ void tripletLoss(float* anchor, float* positive, float* negative, float* loss, int embeddingDimension,
	int nTriplets, float offset)
{
	int tripletIndex = blockIdx.x * blockDim.x + threadIdx.x;
	
	if (tripletIndex < nTriplets)
	{
		float distance_AP = 0.0f; //distance between anchor image and positive image
		float distance_AN = 0.0f; //distance between anchor image and negative image

		for (int i = 0; i < embeddingDimension; ++i)
		{
			float differenceAP = anchor[tripletIndex * embeddingDimension + i] - positive[tripletIndex * embeddingDimension + i];
			float differenceAN = anchor[tripletIndex * embeddingDimension + i] - negative[tripletIndex * embeddingDimension + i];
			distance_AP += differenceAP;
			distance_AN += differenceAN;
		}
		float tripletLoss = fmaxf(0.0f, (distance_AP - distance_AN) + offset);

		loss[tripletIndex] = tripletLoss;
	}
}

void calculateLoss(int numOfTriplets, float* anchorImage, float* positiveImage, float* negativeImage, float* lossArray, int embedDimension, float margin)
{
	int threadsPerBlock = 16;
	int nBlocks = 16;
	int numBlocks = (numOfTriplets + threadsPerBlock - 1) / threadsPerBlock;

	tripletLoss<<<numBlocks, threadsPerBlock>>>(anchorImage, positiveImage, negativeImage, lossArray, embedDimension, numOfTriplets, margin);
}

__global__ void ADAM_OptimizerK(float* weights, float* gradients, float* m,
	float* v, float learningRate, float beta1, float beta2, float epsilon, int t, int size)
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;

	if (index < size)
	{
		//computing first moment and second moment of the gradients (m, v)
		m[index] = beta1 * m[index] + (1 - beta1) * gradients[index];
		v[index] = beta2 * v[index] + (1 - beta2) * gradients[index] * gradients[index];
	
		float m_hat = m[index] / (1 - powf(beta1, t));
		float v_hat = v[index] / (1 - powf(beta2, t));

		weights[index] -= learningRate * m_hat / (sqrtf(v_hat) + epsilon);
	}
}


//this is a custom-made kernel function, which needs its own grid and block dimensions specified when launched on the GPU
__global__ void updateWeights(float* weights, float* gradients, float learningRate, int size) /*SGD optimizer (stochastic gradient descent - simple)*/
{
	//calculating global thread index's x coordinate in the memory block (since weight and gradient arrays are only 1D arrays both in memory representation and visual representation)
	int xIndex = blockIdx.x * blockDim.x + threadIdx.x;
	if (xIndex < size) //no loop - because it will be automatically called inside of a loop in the complicated GPU execution sequence
	{
		weights[xIndex] -= learningRate * gradients[xIndex]; //update each weight with its corresponding gradient.
	}
}

int calculateWeightSize(ConvolutionLayer* layer)
{
	int nOutputChannels, nInputChannels, xKernelDim, yKernelDim;
	CHECK_CUDNN(cudnnGetFilter4dDescriptor(layer->filterDesc, NULL, NULL, &nOutputChannels, &nInputChannels, &xKernelDim, &yKernelDim));
	return(nOutputChannels * nInputChannels * xKernelDim * yKernelDim);
}
int calculateBiasSize(ConvolutionLayer* layer)
{
	int nOutputChannels;
	CHECK_CUDNN(cudnnGetTensor4dDescriptor(layer->biasDesc, NULL, &nOutputChannels, NULL, NULL, NULL, NULL, NULL, NULL, NULL));
	return nOutputChannels;
}

void updateConvolutionLayerWeights(ConvolutionLayer* layer, float learningRate)
{
	int weightSize = calculateWeightSize(layer);
	int biasSize = calculateBiasSize(layer);

	updateWeights<<<(weightSize + 255) / 256, 256 >>>
		(layer->weights, layer->gradWeights, learningRate, weightSize);
	updateWeights<<<(biasSize + 255) / 256, 256 >>>
		(layer->biases, layer->gradBiases, learningRate, biasSize);
}



#endif