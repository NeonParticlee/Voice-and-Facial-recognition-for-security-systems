#ifndef IMAGE_OPS_H
#define IMAGE_OPS_H

#include "cutil.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image_resize2.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



#define RESIZE_DIMENSIONS 224, 224
#ifdef __cplusplus
extern "C" {
#endif


/******************************STB*************************************/
#ifdef GREATER_PRECISION
typedef double dataDtype;
#else
typedef float dataDtype;
#endif //GREATER_PRECISION

#define RED_LUMINANCE 0.3f
#define GREEN_LUMINANCE 0.59f
#define BLUE_LUMINANCE 0.11f

enableVec(dataDtype*, normImageData)
enableVec(char unsigned*, ImgData)

VectornormImageData normalizedImageDataVec = { 0 };

void clearNormData(VectornormImageData* vec)
{
	for (size_t i = 0; i < vec->currEnd; ++i)
	{
		free(vec->data[i]);
		vec->data[i] = NULL;
	}
}

typedef struct
{
	int width;
	int height;
	int channels;
	char unsigned* data;
} stbImage;

typedef struct
{
	int width;
	int height;
	int channels;
	dataDtype* data;

}processed_stbImage;

void printSTBImage(stbImage* img)
{
	printf("width: %d, height: %d, channels: %d\n", img->width, img->height, img->channels);
}



VectorImgData imgData = { 0 }; /*garbage collector for image data, just call loadImage for each directory's image files then after the end of the session call cleanUpRoutine*/

void clearImgData(VectorImgData* v)
{
	for (size_t i = 0; i < v->currEnd; ++i)
	{
		stbi_image_free(v->data[i]);
		v->data[i] = NULL;
	}
}

void loadImage(char* imagePath, stbImage* imgDataHolder)
{

	if (!imagePath) { fprintf(stderr, "image path passed was NULL\n"); return; }

	imgDataHolder->data = stbi_load(imagePath, &imgDataHolder->width, &imgDataHolder->height, &imgDataHolder->channels, 0);
	if (!imgDataHolder->data) { fprintf(stderr, "failed to load image data.\n"); return; }

	vecAddImgData(&imgData, imgDataHolder->data);

}

/****************Image Pre processing******************/
void initProcessedImageParams(processed_stbImage* Image, int width, int height, int channels)
{
	Image->width = width;
	Image->height = height;
	Image->channels = channels; /*3 for RGB, 1 for grayscale*/
	Image->data = (dataDtype*)malloc(sizeof(dataDtype) * (width * height * channels));
	vecAddnormImageData(&normalizedImageDataVec, Image->data);
}

void preProcessImage(stbImage* originalImage, processed_stbImage* newImage)
{
    // if the original image has RGB format we apply grayscale conversion to the pixel data (if in grayscale mode)
    if (originalImage->channels == 3 && newImage->channels == 1)
    {
        size_t monoChannelSize = originalImage->width * originalImage->height;
        
        unsigned char* grayscaleData = (unsigned char*)malloc(monoChannelSize * sizeof(unsigned char));
        
        for (size_t i = 0; i < monoChannelSize; ++i)
        {
            size_t pixel = i * 3;

            // RGB->Grayscale weighted luminance conversion
     unsigned char grayPixelValue = (unsigned char)((originalImage->data[pixel] * RED_LUMINANCE) + (originalImage->data[pixel + 1] * GREEN_LUMINANCE) + (originalImage->data[pixel + 2] * BLUE_LUMINANCE));

            grayscaleData[i] = grayPixelValue;
        }

        
        unsigned char* resizedData = stbir_resize_uint8_linear(grayscaleData, originalImage->width, originalImage->height, 0,
            NULL, newImage->width, newImage->height, 0, (stbir_pixel_layout)newImage->channels);

     //   stbi_write_png("umuuuuuuuu.png", newImage->width, newImage->height, newImage->channels, resizedData, 0);
            

        free(grayscaleData);

        if (resizedData == NULL)
        {
            fprintf(stderr, "RESIZING FAILED----------------------------------\n");
            return;
        }

        
        size_t imgSize = newImage->width * newImage->height * newImage->channels;
        for (size_t i = 0; i < imgSize; ++i)
        {
            newImage->data[i] = resizedData[i] / 255.0f; // Normalize to range [0, 1]
        }

        free(resizedData);
    }
    else
    {
        fprintf(stderr, "Unsupported channel format!\n");
        return;
    }
}


#ifdef __cplusplus
}
#endif

#endif