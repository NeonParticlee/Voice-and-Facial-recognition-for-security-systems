#ifndef _C_VECTOR_H
#define _C_VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef char bool8; //compact return error type
#define EXPANSION_FACTOR 2i8
#define REDUCTION_FACTOR(End, Len) ((End) + (Len))/2.f
#define STRINGIFY(x) #x
#define arrsize(arr) sizeof((arr))/sizeof(arr[0])
#define setSTRING(data) (data), arrsize(data)

#define DONTFIT NULL, 0 /*for usage with vecDeleteDynamic where the user does not want to fit to size upon deletion*/

/*****************STL-like Cvector declarator macro (NON STRUCT TYPES ONLY)*****************/
#define enableVec(dtype, descriptor)		\
typedef struct																																																																				\
{																																																																							\
	dtype* data;																																																																			\
	size_t length;																																																																			\
	size_t currEnd;																																																																			\
} Vector##descriptor;																																																																		\
																																																																							\
		typedef void (*callbackPointerClearer##descriptor)(Vector##descriptor* vec);															\
		typedef void (*callbackPrinterIterative##descriptor)(dtype);																																\
		typedef void (*callbackPrinter##descriptor)(Vector##descriptor* vec);																													\
																															\
																																																																				    \
																																																																						    \
	/******* IMPORTANT NOTES HERE -- PLEASE READ CAREFULLY ********/ 																																																																			    \
																																																																						    \
/*EXAMPLE of a pointer clearer func*/																																																																				\
/*void CLEARER##descriptor(Vector##descriptor* vec)		*/																																																			\
/*{ */																																																																	\
/*	for (size_t i = 0; i < vec->currEnd; ++i)*/																																																						\
/*	{						*/																																																										\
/*		free((dtype*)vec->data[i]);	only free the inner pointers	*/																																																	\
/*	}																*/																																																\
/*}				*/																																																													\
																																																								\
																																																											\
/****************These are the default inner functions, they get called automatically inside of the lib's other functions that need variability in dealing with different data types, initialize the necessary ones of them for your usage purposes**************/		\
			callbackPrinter##descriptor	defaultPrinter##descriptor = NULL;																																																\
			callbackPrinterIterative##descriptor defaultPrinter_It##descriptor = NULL;																																													\
			callbackPointerClearer##descriptor	defaultClearer##descriptor = NULL;	/*used for vectors containing dynamic pointers, like a vector of strings*/																											\
			callbackPointerClearer##descriptor	defaultClearer_L2##descriptor = NULL;	/*used for inner dynamic double pointers, or essentially used for vectors containing double pointer data*/																		\
																																																																		\
																																																																	\
																																																																		\
void initDefaultFunctions##descriptor(callbackPointerClearer##descriptor clearer, callbackPrinter##descriptor printer, callbackPrinterIterative##descriptor printer_it)													\
{																																												\
																														\
		defaultClearer##descriptor = clearer;																																																	\
																																		\
		defaultPrinter##descriptor = printer;																																																	\
																																		\
		defaultPrinter_It##descriptor = printer_it;																																																\
																																													\
}																																																											\
																																																								\
																																																											\
void init_Level2_Pointer_Clearer##descriptor(callbackPointerClearer##descriptor clearer_L2)																																		\
{																																																											\
	defaultClearer_L2##descriptor = clearer_L2;																																																\
}																																																											\
																																																														\
																																																											\
																																																														\
																																																																										\
Vector##descriptor vecCreate##descriptor(dtype* data, size_t length)																																																						\
{																																																																							\
	Vector##descriptor vec = { 0 };																																																															\
																																																																							\
	if (data == NULL || length == 0) return vec;																																																											\
																																																																							\
	size_t bytes = length * (sizeof(dtype));																																																												\
	vec.data = (dtype*)malloc(bytes);																																																														\
	memcpy_s(vec.data, bytes, data, bytes);																																																													\
	vec.length = length;																																																																	\
	vec.currEnd = vec.length;																																																																\
	return vec;																																																																				\
}																																																																							\
																																																																							\
void vecReserve##descriptor(Vector##descriptor* vec, dtype* data, size_t length)																																																			\
{																																																																	\
																																																														\
	dtype* temp = NULL;																																																															\
	if ((vec->currEnd + length) > vec->length)																																																												\
	{																																																																						\
		vec->length += length;																																																																\
		 temp = (dtype*)realloc(vec->data, (vec->length *= EXPANSION_FACTOR) * sizeof(dtype));																																														\
		if (temp)																																																																			\
		{																																																																					\
			vec->data = temp;																																																																\
		}																																																																					\
	}																																																																						\
																																																																							\
	int it = 0;																																																																				\
	for (int i = vec->currEnd; i < (vec->currEnd + length); ++i)																																																							\
	{																																																																						\
		vec->data[i] = data[it++];																																																															\
	}																																																																						\
																																																																						\
	vec->currEnd += length;																																																																	\
}																																																			\
																																																	\
																																																								\
void vecClear##descriptor(Vector##descriptor* vec)																																																											\
{																																																																							\
	if (vec == NULL) return;																																																																\
	free(vec->data);																																																																		\
	vec->length = 0;																																																																		\
	vec->currEnd = 0;																																																																		\
	vec->data = NULL;																																																																		\
}																																																																							\
																																																																							\
void vecClearPointersCustom##descriptor(Vector##descriptor* vec, callbackPointerClearer##descriptor clearer)																																														\
																																																																							\
{																																																																							\
	if (vec == NULL || vec->data == NULL || vec->currEnd == 0 || clearer == NULL) { fprintf(stderr, "no clearers were provided for the function to clear the inner pointers\n"); return; }																																																	\
																																																																							\
	clearer(vec);																																																\
	free(vec->data);/*when passing your clearer to this function, MAKE SURE to implement it so that it only clears the inner pointers (don't touch vec->data) so that runtime memory issues won't occur    */																																																																\
	vec->length = 0;																																																																		\
	vec->currEnd = 0;																																																																		\
	vec->data = NULL;																																																																		\
}																																																																							\
																																																																							\
																																																																								\
void vecClearPointers##descriptor(Vector##descriptor* vec)																																															\
																																																																								\
{																																																																								\
	if (vec == NULL || vec->data == NULL || vec->currEnd == 0) { fprintf(stderr, "no clearers were provided for the function to clear the inner pointers\n"); return; }																										\
	if(defaultClearer##descriptor != NULL)																																																																							\
		defaultClearer##descriptor(vec);		/*for primitives inside of vectors implementation, this will work as well if the default clearer is initialized to NULL, so it can basically FULLy function in place of vecClear*/																																																																		\
	free(vec->data); /*when passing your clearer to this function, MAKE SURE to implement it so that it only clears the inner pointers (don't touch vec->data) so that runtime memory issues won't occur    */																				\
	vec->length = 0;																																																																			\
	vec->currEnd = 0;																																																																			\
	vec->data = NULL;																																																																			\
}																																																																								\
																																																																								\
																																																																								\
Vector##descriptor* vecOverride##descriptor(Vector##descriptor* vec, dtype* data, size_t length)																																				\
{																																																																							\
	if (vec == NULL)																																																																		\
	{																																																																				\
		vec = (Vector##descriptor*)malloc(sizeof(Vector##descriptor));																																																						\
		if (vec != NULL)																																																															\
			*vec = vecCreate##descriptor(data, length);																																																										\
		else fprintf(stderr, "error in allocating memory\n");																																																								\
		return vec;																																																																			\
	}																																																																						\
	else																																																																					\
	{																																																																						\
		if (defaultClearer##descriptor != NULL)	/*works solely by assuming the user didn't define a default clearer to deal with this vector's data (meaning the vector's data is assumed to be of primitive non-pointer type)*/																																																													\
			vecClearPointers##descriptor(vec);																																																										\
		else vecClear##descriptor(vec);																																																														\
	}																																																																						\
	if (data == NULL || length == 0) {return vec;}																																																											\
																																																																				\
	size_t bytes = length * (sizeof(dtype));																																																												\
	vec->data = (dtype*)malloc(bytes);																																																														\
	memcpy_s(vec->data, bytes, data, bytes);																																																												\
	vec->length = length;																																																																	\
	vec->currEnd = vec->length;																																																																\
	return NULL;																																																																			\
																																																																							\
}																																																																							\
																																																																							\
																																																																							\
																																																																									\
Vector##descriptor* vecOverrideCustom##descriptor(Vector##descriptor* vec, callbackPointerClearer##descriptor clearer, dtype* data, size_t length)																																						\
{																																																																									\
	if (vec == NULL)																																																																				\
	{																																																																								\
		vec = (Vector##descriptor*)malloc(sizeof(Vector##descriptor));																																																								\
		if (vec != NULL)																																																																			\
			*vec = vecCreate##descriptor(data, length);																																																												\
		else fprintf(stderr, "error in allocating memory\n");																																																										\
		return vec;																																																																					\
	}																																																																								\
	else																																																																							\
	{																																																																								\
		if (clearer != NULL)																																																																		\
			vecClearPointersCustom##descriptor(vec, clearer);																																																												\
		else vecClearPointers##descriptor(vec);																																																																\
	}																																																																								\
	if (data == NULL || length == 0) { return vec; }																																																												\
																																																																									\
	size_t bytes = length * (sizeof(dtype));																																																														\
	vec->data = (dtype*)malloc(bytes);																																																																\
	memcpy_s(vec->data, bytes, data, bytes);																																																														\
	vec->length = length;																																																																			\
	vec->currEnd = vec->length;																																																																		\
	return NULL;																																																																					\
																																																																									\
}																																																																									\
																																																																									\
void vecAdd##descriptor(Vector##descriptor* vec, dtype data)																																																								\
{																																																																							\
	if (vec->data == NULL)																																																																	\
	{																																																																						\
		vec->currEnd = 0;																																																																	\
		vec->length = 0;																																																																	\
	}																																																																						\
	vec->currEnd += 1;																																																																		\
	if (vec->currEnd > vec->length)																																																															\
	{																																																																						\
		if (vec->length == 0) {																																																																\
			vec->length += 1;																																																																\
		}																																																																					\
		dtype* temp = (dtype*)realloc(vec->data, (vec->length *= EXPANSION_FACTOR) * sizeof(dtype));																																														\
		printf("\nrealloc called. \n");																																																														\
		if (temp == NULL) fprintf(stderr, "ERROR: adding to the vector failed \n");																																																			\
		else																																																																				\
			vec->data = temp;																																																																\
	}																																																																						\
		vec->data[vec->currEnd - 1] = data;																																																													\
		return;																				\
																	\
	fprintf(stderr, "couldn't add to the vector \n");																																																										\
																																																																							\
}																																																																							\
																																																																							\
void vecDelete##descriptor(Vector##descriptor* vec, size_t position)																																																						\
{																													\
	if (vec == NULL || vec->data == NULL || vec->currEnd <= 0 || vec->length <= 1) return;																																																																					\
																															\
	if (position >= vec->currEnd) {																																																															\
		fprintf(stderr, "ERROR in the position that was passed (invalid value). \n");																																																		\
		return;																																																																				\
	}																																																																						\
																																																																							\
	size_t it = 0;																																																																			\
	dtype* newData = (dtype*)malloc(vec->currEnd * sizeof(dtype));																																																							\
	if (newData) {																																																																			\
		for (size_t i = 0; i < vec->currEnd; ++i)																																																											\
		{																																																																					\
			if (i == position)																																																																\
				continue;																																																																	\
			newData[it++] = vec->data[i];																																																													\
		}																																																																					\
	}																																																																						\
	else fprintf(stderr, "error in allocating the copy buffer while deleting. \n");																																																			\
	vec->currEnd -= 1;																																																																		\
	size_t newBytes = vec->currEnd * sizeof(dtype);																																																											\
	memcpy_s(vec->data, newBytes, newData, newBytes);																																																										\
	free(newData);																																																																			\
}																																																																							\
																																																																							\
typedef int (*vecCompFunc##descriptor)(void* a, void* b);																																																									\
																																																																							\
int VecSortAscendingComp##descriptor(void* a, void* b)																																																										\
{																																																																							\
	dtype A = *(dtype*)a;																																																																	\
	dtype B = *(dtype*)b;																																																																	\
																																																																							\
	return ((A > B) - (A < B));																																																																\
}																																																																							\
																																																																							\
int VecSortDescendingComp##descriptor(void* a, void* b)																																																										\
{																																																																							\
	dtype A = *(dtype*)a;																																																																	\
	dtype B = *(dtype*)b;																																																																	\
																																																																							\
	return ((A < B) - (A > B));																																																																\
}																																																																							\
																																																																							\
void vecSort##descriptor(Vector##descriptor* vec, char unsigned ascending)		/*pass 1 if you need to sort the vector ascendingly, and 0 for descending*/																																																			\
{																																																																							\
	vecCompFunc##descriptor comparator = NULL;																																																												\
	if (ascending)																																																																			\
	{																																																																						\
		comparator = VecSortAscendingComp##descriptor;																																																										\
	}																																																																						\
	else																																																																					\
	{																																																																						\
		comparator = VecSortDescendingComp##descriptor;																																																										\
	}																																																																						\
	qsort(vec->data, vec->currEnd, sizeof(vec->data[0]), comparator);																																																						\
}																																																																							\
																																																																							\
void vecReverse##descriptor(Vector##descriptor* vec)																																																										\
{																																																																							\
	if (vec == NULL || vec->data == NULL || vec->length <= 1 || vec->currEnd <= 1) return;																																																	\
	size_t bytes = vec->currEnd * sizeof(dtype);																																																											\
	dtype* arr = (dtype*)malloc(bytes);																																																														\
	memcpy_s(arr, bytes, vec->data, bytes);																																																													\
	if (arr != NULL)																																																																		\
	{	for (size_t i = 0; i < vec->currEnd; ++i)																																																											\
		{																																																																					\
			vec->data[i] = arr[vec->currEnd - i - 1];																																																										\
		}																																																																					\
	free(arr);																																																																				\
	}																																																																													\
}																																																																							\
																																																																							\
void vec_print##descriptor(Vector##descriptor* vec)																																																											\
{																																																																							\
	if (vec == NULL || vec->currEnd == 0 || vec->data == NULL) {printf("Nothing to print for this vector.\n"); return; }																																																						\
	printf("\n\n");																																																																			\
	if (sizeof(dtype) == 1) {																																																																\
		for (int i = 0; i < vec->currEnd; ++i)																																																												\
			printf("%c \n", vec->data[i]);																																																													\
	}																																																																						\
	else {																																																																					\
		for (int i = 0; i < vec->currEnd; ++i)																																																												\
			printf("%f \n", vec->data[i]);																																																													\
	}																																																																						\
	printf("\n\n");																																																																			\
}																																																																							\
																																																																							\
																																																							\
void vecPrintIterativeCustom##descriptor(Vector##descriptor* vec, callbackPrinterIterative##descriptor printer)																																												\
{																																																																							\
	if (vec->currEnd == 0 || vec->data == NULL || printer == NULL) {fprintf(stderr, "could not print using printer iterative for %s.\n", #descriptor); return; }																																																				\
	printf("vector data:  \n");																																																																\
	for (int i = 0; i < vec->currEnd; ++i)																																																													\
	{																																																																						\
		printer(vec->data[i]);																																																																\
	}																																																																						\
	printf("\n");																																																																			\
}																																																																							\
																																																																							\
																																																																								\
void vecPrintIterative##descriptor(Vector##descriptor* vec)																																																										\
{																																																																								\
	if (vec->currEnd == 0 || vec->data == NULL || defaultPrinter_It##descriptor == NULL) { fprintf(stderr, "could not print using printer iterative for %s.\n", #descriptor); return; }																											\
	printf("vector data:  \n");																																																																	\
	for (int i = 0; i < vec->currEnd; ++i)																																																														\
	{																																																																							\
		defaultPrinter_It##descriptor(vec->data[i]);																																																											\
	}																																																																							\
	printf("\n");																																																																				\
}																																																																								\
																																																																								\
																																																																								\
void vecPrintLengths##descriptor(Vector##descriptor* vec)																																																									\
{																																																																							\
	if (vec == NULL) return;																																																																\
	printf("\ncurrent end: %zu, current length: %zu \n", vec->currEnd, vec->length);																																																		\
}																																																																							\
																																																																							\
void vecClearHeapVector##descriptor(Vector##descriptor** vec)																																																								\
{																																																																							\
			vecClearPointers##descriptor(*vec);																																																														\
			if(vec && *vec)																																																																				\
			free(*vec);																																																																											\
			*vec = NULL;																																																																										\
}																																																										\
void vecPrintCustom##descriptor(Vector##descriptor* vec, callbackPrinter##descriptor printer)																																																\
{																																																																							\
	if (vec->currEnd == 0 || vec->data == NULL || printer == NULL) {fprintf(stderr, "could not print using printCustom for: %s.\n", #descriptor); return; }																																																				\
	printer(vec);																																																																		\
	printf("\n");																																																																			\
}																																																																							\
																																																																							\
void vecPrint##descriptor(Vector##descriptor* vec)																																																										\
{																																																																						\
	if (vec->currEnd == 0 || vec->data == NULL || defaultPrinter_It##descriptor == NULL) { fprintf(stderr, "could not print using printCustom for: %s.\n", #descriptor); return; }																										\
																																																										\
	defaultPrinter##descriptor(vec);																																																													\
	printf("\n");																																																																		\
}																																																																						\
																																																																						\
size_t getVecEnd##descriptor(Vector##descriptor* vec)																																																										\
{																																																																							\
	return vec->currEnd;																																																																	\
}																																																																							\
size_t getVecCap##descriptor(Vector##descriptor* vec)																																																										\
{																																																																							\
	return vec->length;																																																																		\
}																																																																						\
																																																																						\
																																																																							\
void vecResize##descriptor(Vector##descriptor* vec, size_t newLen, char unsigned dataLossAllowed)																																			\
{																																																																						\
	dtype* temp = NULL;																																																																	\
	vec->length = newLen;																																																																\
	if (dataLossAllowed)																																																																\
	{																																																																							\
		if (vec->currEnd >= vec->length)																																																												\
			vec->currEnd = vec->length - 1;																																																												\
	}																																																																					\
	else																																																																				\
	{																																																																					\
		if (vec->currEnd > vec->length)																																																													\
			vec->length = vec->currEnd + 1; /*clamp right to the current end (should never happen if the resize is increasing capacity)*/																																				\
		else if(vec->currEnd == vec->length) return;																																																									\
	}																																																																					\
	printf("reallocated on resize\n");																																																												\
	temp = realloc(vec->data, sizeof(dtype) * vec->length);																																																								\
	vecOverride##descriptor(vec, temp, vec->length);																																																							\
	if(temp != NULL)																																																																	\
	free(temp);																																																																			\
}																																																																						\
																																																																						\
																																																																					\
void vecResizeCustom##descriptor(Vector##descriptor* vec, size_t newLen, char unsigned dataLossAllowed, callbackPointerClearer##descriptor clearer)																																	\
{																																																																					\
	dtype* temp = NULL;																																																																\
	vec->length = newLen;																																																															\
	if (dataLossAllowed)																																																															\
	{																																																																				\
		if (vec->currEnd >= vec->length)																																																											\
			vec->currEnd = vec->length - 1;																																																											\
	}																																																																				\
	else																																																																			\
	{																																																																				\
		if (vec->currEnd > vec->length)																																																												\
			vec->length = vec->currEnd + 1; /*clamp right to the current end (should never happen if the resize is increasing capacity)*/																																			\
		else if (vec->currEnd == vec->length) return;																																																								\
	}																																																																				\
	printf("reallocated on resize\n");																																																												\
	temp = realloc(vec->data, sizeof(dtype) * vec->length);																																																							\
	vecOverrideCustom##descriptor(vec, clearer, temp, vec->length);																																																					\
	if (temp != NULL)																																																																\
		free(temp);																																																																	\
}																																																																					\
																																																																					\
																																																																					\
void vecFitToSize##descriptor(Vector##descriptor* vec)																																																									\
{																																																																						\
	if (vec->currEnd < vec->length)																																																														\
	{																																																																					\
		vecResize##descriptor(vec, vec->currEnd, 0);																																																								\
		printf("fitted vector to size\n");																																																												\
	}																																																																					\
	else																																																																				\
		printf("vec does not need resizing (currEnd and length): %d, %d\n", vec->currEnd, vec->length);																																													\
		return;																																																																			\
}																																																																						\
																																																																						\
void vecFitToSizeCustom##descriptor(Vector##descriptor* vec, callbackPointerClearer##descriptor clearer)																																											\
{																																																																						\
	if (vec->currEnd < vec->length)																																																														\
	{																																																																					\
		vecResizeCustom##descriptor(vec, vec->currEnd, 0, clearer);																																																							\
	}																																																																					\
	else																																																																				\
		printf("vec does not need resizing (currEnd and length): %d, %d\n", vec->currEnd, vec->length);																																													\
	return;																																																																				\
}																																																																						\
																																																																						\
void vecDeleteDynamic##descriptor(Vector##descriptor* vec, size_t position) /*more memory aware version of vecDelete, resizes vector downward accordingly*/														\
{																																																																						\
	if (vec == NULL || vec->data == NULL || vec->currEnd <= 0 || vec->length <= 1) return;																																																\
																																																																						\
	if (position >= vec->currEnd) {																																																														\
		fprintf(stderr, "ERROR in the position that was passed (invalid value). \n");																																																	\
		return;																																																																			\
	}																																																																					\
																																																																						\
																																																																						\
	size_t it = 0;																																																																		\
	if (vec->currEnd != 0 && ((vec->currEnd*2) < vec->length))																																																							\
	{																																																																					\
		vec->length /= 2;																																																																\
		vecResize##descriptor(vec, vec->length, 0);																																																							\
	}																																																																					\
	dtype* newData = (dtype*)malloc(vec->currEnd * sizeof(dtype));																																																						\
	if (newData) {																																																																		\
		for (size_t i = 0; i < vec->currEnd; ++i)																																																										\
		{																																																																				\
			if (i == position)																																																															\
				continue;																																																																\
			newData[it++] = vec->data[i];																																																												\
		}																																																																				\
	}																																																																					\
	else fprintf(stderr, "error in allocating the copy buffer while deleting. \n");																																																		\
	vec->currEnd -= 1;																																																																	\
	size_t newBytes = vec->currEnd * sizeof(dtype);																																																										\
	memcpy_s(vec->data, newBytes, newData, newBytes);																																																									\
	free(newData);																																																																		\
}																																																																						\
																																																																						\
																																																																						\
void vecDeleteDynamicCustom##descriptor(Vector##descriptor* vec, size_t position, callbackPointerClearer##descriptor clearer) /*more memory aware version of vecDelete, resizes vector downward accordingly*/														\
{																																																																						\
	if (vec == NULL || vec->data == NULL || vec->currEnd <= 0 || vec->length <= 1) return;																																																\
																																																																						\
	if (position >= vec->currEnd) {																																																														\
		fprintf(stderr, "ERROR in the position that was passed (invalid value). \n");																																																	\
		return;																																																																			\
	}																																																																					\
																																																																						\
																																																																						\
	size_t it = 0;																																																																		\
	if (vec->currEnd != 0 && ((vec->currEnd * 2) < vec->length))																																																						\
	{																																																																					\
		vec->length /= 2;																																																																\
		vecResize##descriptor(vec, vec->length, clearer, 0);																																																							\
	}																																																																					\
	dtype* newData = (dtype*)malloc(vec->currEnd * sizeof(dtype));																																																						\
	if (newData) {																																																																		\
		for (size_t i = 0; i < vec->currEnd; ++i)																																																										\
		{																																																																				\
			if (i == position)																																																															\
				continue;																																																																\
			newData[it++] = vec->data[i];																																																												\
		}																																																																				\
	}																																																																					\
	else fprintf(stderr, "error in allocating the copy buffer while deleting. \n");																																																		\
	vec->currEnd -= 1;																																																																	\
	size_t newBytes = vec->currEnd * sizeof(dtype);																																																										\
	memcpy_s(vec->data, newBytes, newData, newBytes);																																																									\
	free(newData);																																																																		\
}																																																																						\
																																																																						\
																																																																						\
Vector##descriptor vecCopy##descriptor(Vector##descriptor* vecToBeCopied)																																																				\
{																																																																						\
	if (vecToBeCopied == NULL) return;																																																													\
	Vector##descriptor vec = { 0 };																																																														\
	vec.length = vecToBeCopied->length;																																																													\
	vec.currEnd = vecToBeCopied->currEnd;																																																												\
																																																																						\
	if (vecToBeCopied->data != NULL)																																																													\
		memcpy_s(vec.data, vec.currEnd, vecToBeCopied->data, vecToBeCopied->currEnd);																																																	\
																																																																						\
	else vec.data = (dtype*)NULL;																																																														\
																																																																						\
	return vec;																																																																			\
}																																																																						\
																																																																						\
dtype vecPop##descriptor(Vector##descriptor* vec)																																																										\
{																																																																						\
	if (vec == NULL || vec->data == NULL) { fprintf(stderr, "nothing to be popped\n"); return *(dtype*)NULL; }																																													\
																																																																						\
	dtype data = vec->data[vec->currEnd - 1];																																																											\
																																																																						\
	vec->currEnd -= 1;																																																																	\
																																																																						\
	return data;																																																																		\
}

/********************************************************************************************************************************************************************************************************************************************************************************************************/
/********************************************************************************************************************************************************************************************************************************************************************************************************/
/********************************************************************************************************************************************************************************************************************************************************************************************************/
/********************************************************************************************************************************************************************************************************************************************************************************************************/

																/***********	STRUCT VECTOR ENABLER	**********************/
/* This macro enables enables structs containing vectors, the catch is that it doesn't allow sorting functionality by default on its data. */
#define enableStructVec(dtype, descriptor)		\
typedef struct																																																																				\
{																																																																							\
	dtype* data;																																																																			\
	size_t length;																																																																			\
	size_t currEnd;																																																																			\
} Vector##descriptor;																																																																		\
																																																																							\
		typedef void (*callbackPointerClearer##descriptor)(Vector##descriptor* vec);															\
		typedef void (*callbackPrinterIterative##descriptor)(dtype);																																\
		typedef void (*callbackPrinter##descriptor)(Vector##descriptor* vec);																													\
																															\
																																																																				    \
																																																																						    \
	/******* IMPORTANT NOTES HERE -- PLEASE READ CAREFULLY ********/ 																																																																			    \
																																																																						    \
/*EXAMPLE of a pointer clearer func*/																																																																				\
/*void CLEARER##descriptor(Vector##descriptor* vec)		*/																																																			\
/*{ */																																																																	\
/*	for (size_t i = 0; i < vec->currEnd; ++i)*/																																																						\
/*	{						*/																																																										\
/*		free((dtype*)vec->data[i]);	only free the inner pointers	*/																																																	\
/*	}																*/																																																\
/*}				*/																																																													\
																																																								\
																																																											\
/****************These are the default inner functions, they get called automatically inside of the lib's other functions that need variability in dealing with different data types, initialize the necessary ones of them for your usage purposes**************/		\
			callbackPrinter##descriptor	defaultPrinter##descriptor = NULL;																																																\
			callbackPrinterIterative##descriptor defaultPrinter_It##descriptor = NULL;																																													\
			callbackPointerClearer##descriptor	defaultClearer##descriptor = NULL;	/*used for vectors containing dynamic pointers, like a vector of strings*/																											\
			callbackPointerClearer##descriptor	defaultClearer_L2##descriptor = NULL;	/*used for inner dynamic double pointers, or essentially used for vectors containing double pointer data*/																		\
																																																																		\
																																																																	\
																																																																		\
void initDefaultFunctions##descriptor(callbackPointerClearer##descriptor clearer, callbackPrinter##descriptor printer, callbackPrinterIterative##descriptor printer_it)													\
{																																												\
																														\
		defaultClearer##descriptor = clearer;																																																	\
																																		\
		defaultPrinter##descriptor = printer;																																																	\
																																		\
		defaultPrinter_It##descriptor = printer_it;																																																\
																																													\
}																																																											\
																																																								\
																																																											\
void init_Level2_Pointer_Clearer##descriptor(callbackPointerClearer##descriptor clearer_L2)																																		\
{																																																											\
	defaultClearer_L2##descriptor = clearer_L2;																																																\
}																																																											\
																																																														\
																																																											\
																																																														\
																																																																										\
Vector##descriptor vecCreate##descriptor(dtype* data, size_t length)																																																						\
{																																																																							\
	Vector##descriptor vec = { 0 };																																																															\
																																																																							\
	if (data == NULL || length == 0) return vec;																																																											\
																																																																							\
	size_t bytes = length * (sizeof(dtype));																																																												\
	vec.data = (dtype*)malloc(bytes);																																																														\
	memcpy_s(vec.data, bytes, data, bytes);																																																													\
	vec.length = length;																																																																	\
	vec.currEnd = vec.length;																																																																\
	return vec;																																																																				\
}																																																																							\
																																																																							\
void vecReserve##descriptor(Vector##descriptor* vec, dtype* data, size_t length)																																																			\
{																																																																	\
																																																														\
	dtype* temp = NULL;																																																															\
	if ((vec->currEnd + length) > vec->length)																																																												\
	{																																																																						\
		vec->length += length;																																																																\
		 temp = (dtype*)realloc(vec->data, (vec->length *= EXPANSION_FACTOR) * sizeof(dtype));																																														\
		if (temp)																																																																			\
		{																																																																					\
			vec->data = temp;																																																																\
		}																																																																					\
	}																																																																						\
																																																																							\
	int it = 0;																																																																				\
	for (int i = vec->currEnd; i < (vec->currEnd + length); ++i)																																																							\
	{																																																																						\
		vec->data[i] = data[it++];																																																															\
	}																																																																						\
																																																																						\
	vec->currEnd += length;																																																																	\
}																																																			\
																																																	\
																																																								\
void vecClear##descriptor(Vector##descriptor* vec)																																																											\
{																																																																							\
	if (vec == NULL) return;																																																																\
	free(vec->data);																																																																		\
	vec->length = 0;																																																																		\
	vec->currEnd = 0;																																																																		\
	vec->data = NULL;																																																																		\
}																																																																							\
																																																																							\
void vecClearPointersCustom##descriptor(Vector##descriptor* vec, callbackPointerClearer##descriptor clearer)																																														\
																																																																							\
{																																																																							\
	if (vec == NULL || vec->data == NULL || vec->currEnd == 0 || clearer == NULL) { fprintf(stderr, "no clearers were provided for the function to clear the inner pointers\n"); return; }																																																	\
																																																																							\
	clearer(vec);																																																\
	free(vec->data);/*when passing your clearer to this function, MAKE SURE to implement it so that it only clears the inner pointers (don't touch vec->data) so that runtime memory issues won't occur    */																																																																\
	vec->length = 0;																																																																		\
	vec->currEnd = 0;																																																																		\
	vec->data = NULL;																																																																		\
}																																																																							\
																																																																							\
																																																																								\
void vecClearPointers##descriptor(Vector##descriptor* vec)																																															\
																																																																								\
{																																																																								\
	if (vec == NULL || vec->data == NULL || vec->currEnd == 0) { fprintf(stderr, "no clearers were provided for the function to clear the inner pointers\n"); return; }																										\
	if(defaultClearer##descriptor != NULL)																																																																							\
		defaultClearer##descriptor(vec);		/*for primitives inside of vectors implementation, this will work as well if the default clearer is initialized to NULL, so it can basically FULLy function in place of vecClear*/																																																																		\
	free(vec->data); /*when passing your clearer to this function, MAKE SURE to implement it so that it only clears the inner pointers (don't touch vec->data) so that runtime memory issues won't occur    */																				\
	vec->length = 0;																																																																			\
	vec->currEnd = 0;																																																																			\
	vec->data = NULL;																																																																			\
}																																																																								\
																																																																								\
																																																																								\
Vector##descriptor* vecOverride##descriptor(Vector##descriptor* vec, dtype* data, size_t length)																																				\
{																																																																							\
	if (vec == NULL)																																																																		\
	{																																																																				\
		vec = (Vector##descriptor*)malloc(sizeof(Vector##descriptor));																																																						\
		if (vec != NULL)																																																															\
			*vec = vecCreate##descriptor(data, length);																																																										\
		else fprintf(stderr, "error in allocating memory\n");																																																								\
		return vec;																																																																			\
	}																																																																						\
	else																																																																					\
	{																																																																						\
		if (defaultClearer##descriptor != NULL)	/*works solely by assuming the user didn't define a default clearer to deal with this vector's data (meaning the vector's data is assumed to be of primitive non-pointer type)*/																																																													\
			vecClearPointers##descriptor(vec);																																																										\
		else vecClear##descriptor(vec);																																																														\
	}																																																																						\
	if (data == NULL || length == 0) {return vec;}																																																											\
																																																																				\
	size_t bytes = length * (sizeof(dtype));																																																												\
	vec->data = (dtype*)malloc(bytes);																																																														\
	memcpy_s(vec->data, bytes, data, bytes);																																																												\
	vec->length = length;																																																																	\
	vec->currEnd = vec->length;																																																																\
	return NULL;																																																																			\
																																																																							\
}																																																																							\
																																																																							\
																																																																							\
																																																																									\
Vector##descriptor* vecOverrideCustom##descriptor(Vector##descriptor* vec, callbackPointerClearer##descriptor clearer, dtype* data, size_t length)																																						\
{																																																																									\
	if (vec == NULL)																																																																				\
	{																																																																								\
		vec = (Vector##descriptor*)malloc(sizeof(Vector##descriptor));																																																								\
		if (vec != NULL)																																																																			\
			*vec = vecCreate##descriptor(data, length);																																																												\
		else fprintf(stderr, "error in allocating memory\n");																																																										\
		return vec;																																																																					\
	}																																																																								\
	else																																																																							\
	{																																																																								\
		if (clearer != NULL)																																																																		\
			vecClearPointersCustom##descriptor(vec, clearer);																																																												\
		else vecClearPointers##descriptor(vec);																																																																\
	}																																																																								\
	if (data == NULL || length == 0) { return vec; }																																																												\
																																																																									\
	size_t bytes = length * (sizeof(dtype));																																																														\
	vec->data = (dtype*)malloc(bytes);																																																																\
	memcpy_s(vec->data, bytes, data, bytes);																																																														\
	vec->length = length;																																																																			\
	vec->currEnd = vec->length;																																																																		\
	return NULL;																																																																					\
																																																																									\
}																																																																									\
																																																																									\
void vecAdd##descriptor(Vector##descriptor* vec, dtype data)																																																								\
{																																																																							\
	if (vec->data == NULL)																																																																	\
	{																																																																						\
		vec->currEnd = 0;																																																																	\
		vec->length = 0;																																																																	\
	}																																																																						\
	vec->currEnd += 1;																																																																		\
	if (vec->currEnd > vec->length)																																																															\
	{																																																																						\
		if (vec->length == 0) {																																																																\
			vec->length += 1;																																																																\
		}																																																																					\
		dtype* temp = (dtype*)realloc(vec->data, (vec->length *= EXPANSION_FACTOR) * sizeof(dtype));																																														\
		printf("\nrealloc called. \n");																																																														\
		if (temp == NULL) fprintf(stderr, "ERROR: adding to the vector failed \n");																																																			\
		else																																																																				\
			vec->data = temp;																																																																\
	}																																																																						\
		vec->data[vec->currEnd - 1] = data;																																																													\
		return;																				\
																	\
	fprintf(stderr, "couldn't add to the vector \n");																																																										\
																																																																							\
}																																																																							\
																																																																							\
void vecDelete##descriptor(Vector##descriptor* vec, size_t position)																																																						\
{																													\
	if (vec == NULL || vec->data == NULL || vec->currEnd <= 0 || vec->length <= 1) return;																																																																					\
																															\
	if (position >= vec->currEnd) {																																																															\
		fprintf(stderr, "ERROR in the position that was passed (invalid value). \n");																																																		\
		return;																																																																				\
	}																																																																						\
																																																																							\
	size_t it = 0;																																																																			\
	dtype* newData = (dtype*)malloc(vec->currEnd * sizeof(dtype));																																																							\
	if (newData) {																																																																			\
		for (size_t i = 0; i < vec->currEnd; ++i)																																																											\
		{																																																																					\
			if (i == position)																																																																\
				continue;																																																																	\
			newData[it++] = vec->data[i];																																																													\
		}																																																																					\
	}																																																																						\
	else fprintf(stderr, "error in allocating the copy buffer while deleting. \n");																																																			\
	vec->currEnd -= 1;																																																																		\
	size_t newBytes = vec->currEnd * sizeof(dtype);																																																											\
	memcpy_s(vec->data, newBytes, newData, newBytes);																																																										\
	free(newData);																																																																			\
}																																																																							\
																																																																							\
typedef int (*vecCompFunc##descriptor)(void* a, void* b);																																																									\
																																																																							\
			vecCompFunc##descriptor VecSortAscendingComp##descriptor = NULL;																																																											\
			vecCompFunc##descriptor	VecSortDescendingComp##descriptor = NULL;																																																														\
																																																																							\
void vecSort##descriptor(Vector##descriptor* vec, char unsigned ascending)		/*pass 1 if you need to sort the vector ascendingly, and 0 for descending*/																																																			\
{																																																																							\
	vecCompFunc##descriptor comparator = NULL;																																																												\
	if (ascending && VecSortAscendingComp##descriptor)																																																																			\
	{																																																																						\
		comparator = VecSortAscendingComp##descriptor;																																																										\
	}																																																																						\
	else if(VecSortDescendingComp##descriptor)																																																																					\
	{																																																																						\
		comparator = VecSortDescendingComp##descriptor;																																																										\
	}																																																																						\
	else {fprintf(stderr, "you cannot sort your vector of structs right now, please implement proper comparison functions for it first.\n"); return;}																	\
	qsort(vec->data, vec->currEnd, sizeof(vec->data[0]), comparator);																																																						\
}																																																																							\
																																																																							\
void vecReverse##descriptor(Vector##descriptor* vec)																																																										\
{																																																																							\
	if (vec == NULL || vec->data == NULL || vec->length <= 1 || vec->currEnd <= 1) return;																																																	\
	size_t bytes = vec->currEnd * sizeof(dtype);																																																											\
	dtype* arr = (dtype*)malloc(bytes);																																																														\
	memcpy_s(arr, bytes, vec->data, bytes);																																																													\
	if (arr != NULL)																																																																		\
	{	for (size_t i = 0; i < vec->currEnd; ++i)																																																											\
		{																																																																					\
			vec->data[i] = arr[vec->currEnd - i - 1];																																																										\
		}																																																																					\
	free(arr);																																																																				\
	}																																																																													\
}																																																																							\
																																																																							\
void vec_print##descriptor(Vector##descriptor* vec)																																																											\
{																																																																							\
	if (vec == NULL || vec->currEnd == 0 || vec->data == NULL) {printf("Nothing to print for this vector.\n"); return; }																																																						\
	printf("\n\n");																																																																			\
	if (sizeof(dtype) == 1) {																																																																\
		for (int i = 0; i < vec->currEnd; ++i)																																																												\
			printf("%c \n", vec->data[i]);																																																													\
	}																																																																						\
	else {																																																																					\
		for (int i = 0; i < vec->currEnd; ++i)																																																												\
			printf("%f \n", vec->data[i]);																																																													\
	}																																																																						\
	printf("\n\n");																																																																			\
}																																																																							\
																																																																							\
																																																							\
void vecPrintIterativeCustom##descriptor(Vector##descriptor* vec, callbackPrinterIterative##descriptor printer)																																												\
{																																																																							\
	if (vec->currEnd == 0 || vec->data == NULL || printer == NULL) {fprintf(stderr, "could not print using printer iterative for %s.\n", #descriptor); return; }																																																				\
	printf("vector data:  \n");																																																																\
	for (int i = 0; i < vec->currEnd; ++i)																																																													\
	{																																																																						\
		printer(vec->data[i]);																																																																\
	}																																																																						\
	printf("\n");																																																																			\
}																																																																							\
																																																																							\
																																																																								\
void vecPrintIterative##descriptor(Vector##descriptor* vec)																																																										\
{																																																																								\
	if (vec->currEnd == 0 || vec->data == NULL || defaultPrinter_It##descriptor == NULL) { fprintf(stderr, "could not print using printer iterative for %s.\n", #descriptor); return; }																											\
	printf("vector data:  \n");																																																																	\
	for (int i = 0; i < vec->currEnd; ++i)																																																														\
	{																																																																							\
		defaultPrinter_It##descriptor(vec->data[i]);	printf("\n");																																																										\
	}																																																																							\
	printf("\n");																																																																				\
}																																																																								\
																																																																								\
																																																																								\
void vecPrintLengths##descriptor(Vector##descriptor* vec)																																																									\
{																																																																							\
	if (vec == NULL) return;																																																																\
	printf("\ncurrent end: %zu, current length: %zu \n", vec->currEnd, vec->length);																																																		\
}																																																																							\
																																																																							\
void vecClearHeapVector##descriptor(Vector##descriptor** vec)																																																								\
{																																																																							\
	vecClearPointers##descriptor(*vec);																																																																\
	if(vec && *vec)																																	\
	free(*vec);																														\
	*vec = NULL;																																																																				\
}																																																																							\
																																																																							\
																																																										\
void vecPrintCustom##descriptor(Vector##descriptor* vec, callbackPrinter##descriptor printer)																																																\
{																																																																							\
	if (vec->currEnd == 0 || vec->data == NULL || printer == NULL) {fprintf(stderr, "could not print using printCustom for: %s.\n", #descriptor); return; }																																																				\
	printer(vec);																																																																		\
	printf("\n");																																																																			\
}																																																																							\
																																																																							\
void vecPrint##descriptor(Vector##descriptor* vec)																																																										\
{																																																																						\
	if (vec->currEnd == 0 || vec->data == NULL || defaultPrinter_It##descriptor == NULL) { fprintf(stderr, "could not print using printCustom for: %s.\n", #descriptor); return; }																										\
																																																										\
	defaultPrinter##descriptor(vec);																																																													\
	printf("\n");																																																																		\
}																																																																						\
																																																																						\
size_t getVecEnd##descriptor(Vector##descriptor* vec)																																																										\
{																																																																							\
	return vec->currEnd;																																																																	\
}																																																																							\
size_t getVecCap##descriptor(Vector##descriptor* vec)																																																										\
{																																																																							\
	return vec->length;																																																																		\
}																																																																						\
																																																																						\
																																																																							\
void vecResize##descriptor(Vector##descriptor* vec, size_t newLen, char unsigned dataLossAllowed)																																			\
{																																																																						\
	dtype* temp = NULL;																																																																	\
	vec->length = newLen;																																																																\
	if (dataLossAllowed)																																																																\
	{																																																																							\
		if (vec->currEnd >= vec->length)																																																												\
			vec->currEnd = vec->length - 1;																																																												\
	}																																																																					\
	else																																																																				\
	{																																																																					\
		if (vec->currEnd > vec->length)																																																													\
			vec->length = vec->currEnd + 1; /*clamp right to the current end (should never happen if the resize is increasing capacity)*/																																				\
		else if(vec->currEnd == vec->length) return;																																																									\
	}																																																																					\
	printf("reallocated on resize\n");																																																												\
	temp = realloc(vec->data, sizeof(dtype) * vec->length);																																																								\
	vecOverride##descriptor(vec, temp, vec->length);																																																							\
	if(temp != NULL)																																																																	\
	free(temp);																																																																			\
}																																																																						\
																																																																						\
																																																																					\
void vecResizeCustom##descriptor(Vector##descriptor* vec, size_t newLen, char unsigned dataLossAllowed, callbackPointerClearer##descriptor clearer)																																	\
{																																																																					\
	dtype* temp = NULL;																																																																\
	vec->length = newLen;																																																															\
	if (dataLossAllowed)																																																															\
	{																																																																				\
		if (vec->currEnd >= vec->length)																																																											\
			vec->currEnd = vec->length - 1;																																																											\
	}																																																																				\
	else																																																																			\
	{																																																																				\
		if (vec->currEnd > vec->length)																																																												\
			vec->length = vec->currEnd + 1; /*clamp right to the current end (should never happen if the resize is increasing capacity)*/																																			\
		else if (vec->currEnd == vec->length) return;																																																								\
	}																																																																				\
	printf("reallocated on resize\n");																																																												\
	temp = realloc(vec->data, sizeof(dtype) * vec->length);																																																							\
	vecOverrideCustom##descriptor(vec, clearer, temp, vec->length);																																																					\
	if (temp != NULL)																																																																\
		free(temp);																																																																	\
}																																																																					\
																																																																					\
																																																																					\
void vecFitToSize##descriptor(Vector##descriptor* vec)																																																									\
{																																																																						\
	if (vec->currEnd < vec->length)																																																														\
	{																																																																					\
		vecResize##descriptor(vec, vec->currEnd, 0);																																																								\
		printf("fitted vector to size\n");																																																												\
	}																																																																					\
	else																																																																				\
		printf("vec does not need resizing (currEnd and length): %d, %d\n", vec->currEnd, vec->length);																																													\
		return;																																																																			\
}																																																																						\
																																																																						\
void vecFitToSizeCustom##descriptor(Vector##descriptor* vec, callbackPointerClearer##descriptor clearer)																																											\
{																																																																						\
	if (vec->currEnd < vec->length)																																																														\
	{																																																																					\
		vecResizeCustom##descriptor(vec, vec->currEnd, 0, clearer);																																																							\
	}																																																																					\
	else																																																																				\
		printf("vec does not need resizing (currEnd and length): %d, %d\n", vec->currEnd, vec->length);																																													\
	return;																																																																				\
}																																																																						\
																																																																						\
void vecDeleteDynamic##descriptor(Vector##descriptor* vec, size_t position) /*more memory aware version of vecDelete, resizes vector downward accordingly*/														\
{																																																																						\
	if (vec == NULL || vec->data == NULL || vec->currEnd <= 0 || vec->length <= 1) return;																																																\
																																																																						\
	if (position >= vec->currEnd) {																																																														\
		fprintf(stderr, "ERROR in the position that was passed (invalid value). \n");																																																	\
		return;																																																																			\
	}																																																																					\
																																																																						\
																																																																						\
	size_t it = 0;																																																																		\
	if (vec->currEnd != 0 && ((vec->currEnd*2) < vec->length))																																																							\
	{																																																																					\
		vec->length /= 2;																																																																\
		vecResize##descriptor(vec, vec->length, 0);																																																							\
	}																																																																					\
	dtype* newData = (dtype*)malloc(vec->currEnd * sizeof(dtype));																																																						\
	if (newData) {																																																																		\
		for (size_t i = 0; i < vec->currEnd; ++i)																																																										\
		{																																																																				\
			if (i == position)																																																															\
				continue;																																																																\
			newData[it++] = vec->data[i];																																																												\
		}																																																																				\
	}																																																																					\
	else fprintf(stderr, "error in allocating the copy buffer while deleting. \n");																																																		\
	vec->currEnd -= 1;																																																																	\
	size_t newBytes = vec->currEnd * sizeof(dtype);																																																										\
	memcpy_s(vec->data, newBytes, newData, newBytes);																																																									\
	free(newData);																																																																		\
}																																																																						\
																																																																						\
																																																																						\
void vecDeleteDynamicCustom##descriptor(Vector##descriptor* vec, size_t position, callbackPointerClearer##descriptor clearer) /*more memory aware version of vecDelete, resizes vector downward accordingly*/														\
{																																																																						\
	if (vec == NULL || vec->data == NULL || vec->currEnd <= 0 || vec->length <= 1) return;																																																\
																																																																						\
	if (position >= vec->currEnd) {																																																														\
		fprintf(stderr, "ERROR in the position that was passed (invalid value). \n");																																																	\
		return;																																																																			\
	}																																																																					\
																																																																						\
																																																																						\
	size_t it = 0;																																																																		\
	if (vec->currEnd != 0 && ((vec->currEnd * 2) < vec->length))																																																						\
	{																																																																					\
		vec->length /= 2;																																																																\
		vecResize##descriptor(vec, vec->length, clearer, 0);																																																							\
	}																																																																					\
	dtype* newData = (dtype*)malloc(vec->currEnd * sizeof(dtype));																																																						\
	if (newData) {																																																																		\
		for (size_t i = 0; i < vec->currEnd; ++i)																																																										\
		{																																																																				\
			if (i == position)																																																															\
				continue;																																																																\
			newData[it++] = vec->data[i];																																																												\
		}																																																																				\
	}																																																																					\
	else fprintf(stderr, "error in allocating the copy buffer while deleting. \n");																																																		\
	vec->currEnd -= 1;																																																																	\
	size_t newBytes = vec->currEnd * sizeof(dtype);																																																										\
	memcpy_s(vec->data, newBytes, newData, newBytes);																																																									\
	free(newData);																																																																		\
}																																																																						\
																																																																						\
																																																																						\
Vector##descriptor vecCopy##descriptor(Vector##descriptor* vecToBeCopied)																																																				\
{																																																																						\
	if (vecToBeCopied == NULL) return;																																																													\
	Vector##descriptor vec = { 0 };																																																														\
	vec.length = vecToBeCopied->length;																																																													\
	vec.currEnd = vecToBeCopied->currEnd;																																																												\
																																																																						\
	if (vecToBeCopied->data != NULL)																																																													\
		memcpy_s(vec.data, vec.currEnd, vecToBeCopied->data, vecToBeCopied->currEnd);																																																	\
																																																																						\
	else vec.data = (dtype*)NULL;																																																														\
																																																																						\
	return vec;																																																																			\
}																																																																						\
																																																																						\
dtype vecPop##descriptor(Vector##descriptor* vec)																																																										\
{																																																																						\
	if (vec == NULL || vec->data == NULL) { fprintf(stderr, "nothing to be popped\n"); return *(dtype*)NULL; }																																													\
																																																																						\
	dtype data = vec->data[vec->currEnd - 1];																																																											\
																																																																						\
	vec->currEnd -= 1;																																																																	\
																																																																						\
	return data;																																																																		\
}

#endif
