#ifndef FILE_OPS_H
#define FILE_OPS_H
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include<stdlib.h>

typedef long long int lld;

char* FileOperation(const char* filePath, const char* openMode, const char* BufferToWriteFrom)
{
	FILE* fileP = NULL;
	char* readBuff = NULL;

	if ((fileP = fopen(filePath, openMode)) == NULL) //in case the user decides to append and the file does not yet exist
	{
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
		//dynamicStrings.push_back(readBuff); implement the cvec here once everything is good
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
#endif
