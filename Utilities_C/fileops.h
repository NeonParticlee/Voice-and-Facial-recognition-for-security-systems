#ifndef FILE_OPS_H
#define FILE_OPS_H
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

/*all the garbage collection vectors in the utility libs will be prefixed by GC_ (Garbage Collector)*/

#include <stdio.h>
#include <stdlib.h>

#include "cvector.h"

#ifdef __cplusplus
extern "C" {
#endif



enableVec(void*, FileOpsVector)

VectorFileOpsVector GC_fOps = { 0 };

void FOPS_CLEARER(VectorFileOpsVector* fVec)
{
	
	for (size_t i = 0; i < fVec->currEnd; ++i)
	{
		free(fVec->data[i]);
	}
}



void FileOpsCleanUpSession() /*call once at the end of each frame if every single frame for example in a loop executes,
or generally at the total end of each session where you do not need to call any other function again from that lib.*/
{
	if (defaultClearerFileOpsVector == NULL)
		defaultClearerFileOpsVector = FOPS_CLEARER;

	vecClearPointersFileOpsVector(&GC_fOps);
}

#define MAXPATH 256

#define ARR_SIZE(arr) (sizeof((arr))/sizeof((arr)[0]))
/*macros for facilitating passing write arrays, pointers, and vectors to the fileoperationV function which deals with void data*/
#define passWriteBuffer(buff) (void*)buff, sizeof(buff[0]), sizeof(buff)/sizeof(buff[0]) /*works only for stack buffers*/
#define passWriteCharBuffer(buff) (void*)buff, sizeof(buff[0]), sizeof(buff)-1
#define passWritePointer(buff, len) (void*)buff, sizeof(buff[0]), len
#define passWriteVec(vec) (void*)vec.data, sizeof(vec.data[0]), vec.currEnd
#define passWriteNULL NULL, (size_t)NULL, (size_t)NULL



typedef long long int lld;

char* FileOperationString(const char* filePath, const char* openMode, const char* BufferToWriteFrom)
{
	FILE* fileP = NULL;
	char* readBuff = NULL;

	if ((fileP = fopen(filePath, openMode)) == NULL) //in case the user decides to append and the file does not yet exist
	{
		fprintf(stderr, "file could not be found\n");
		return (char*)NULL;
	}
	if (openMode[0] == 'r') //read mode in any of its forms (rb or r), will not use the last parameter in this case
	{
		fseek(fileP, 0, SEEK_END);
		lld fileSize = (lld)ftell(fileP);
		rewind(fileP);
		readBuff = (char*)malloc((sizeof(char)) * (fileSize + 1));
		if (readBuff == NULL)
		{
			fclose(fileP);
			return NULL;
		}
		char currChar;
		size_t i = 0;
		while (((currChar = getc(fileP)) != EOF) && (i < fileSize))
		{
			readBuff[i++] = currChar;
		}
		fclose(fileP);
		readBuff[i] = '\0';

		vecAddFileOpsVector(&GC_fOps, (void*)readBuff);

		return readBuff;
	}
	else //write mode whether it be append or write
	{
		for (int i = 0; (BufferToWriteFrom[i] != '\0'); ++i)
		{
			fputc(BufferToWriteFrom[i], fileP);
		}
		fclose(fileP);
	}
	return (char*)"c\0";
}



void* FileOperationRaw(const char* filePath, const char* openMode, void* BufferToWriteFrom, size_t elemSize, size_t length)
{
	FILE* fileP = NULL;
	char* readBuff = NULL;

	if ((fileP = fopen(filePath, openMode)) == NULL) //in case the user decides to append and the file does not yet exist
	{
		fprintf(stderr, "file could not be found\n");
		return NULL;
	}
	if (openMode[0] == 'r') //read mode in any of its forms (rb or r), will not use the last parameter in this case
	{
		fseek(fileP, 0, SEEK_END);

		lld fileSize = (lld)ftell(fileP);

		rewind(fileP);

		readBuff = (char*)malloc(sizeof(char)*(fileSize + 1));
		
		if (readBuff == NULL)
		{
			fclose(fileP);
			return NULL;
		}
		char currChar;
		size_t i = 0;

		fread(readBuff, elemSize, fileSize, fileP);

		fclose(fileP);

		vecAddFileOpsVector(&GC_fOps, (void*)readBuff);

		return readBuff;
	}

	else //write mode whether it be append or write
	{
		if (!length || !elemSize) { fprintf(stderr, "There was no specified length for the buffer that was passed to write from\n"); return NULL; }

		fwrite(BufferToWriteFrom, elemSize, length, fileP);
		
		fclose(fileP);
	}

	return NULL;
}

#ifdef __cplusplus
}
#endif

#endif