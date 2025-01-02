#ifndef DYNAMIC_STRINGS_H
#define DYNAMIC_STRINGS_H
#include "cvector.h"
#include <stdarg.h>

#define STRINGIFY(x) #x

#ifdef __cplusplus
extern "C" {
#endif



enableVec(char*, DSTR)

VectorDSTR GC_dStr = { 0 };

void DSTR_CLEARER(VectorDSTR* sVec)
{
	
	for (size_t i = 0; i < sVec->currEnd; ++i)
	{
		free(sVec->data[i]);
	}
}


void dynamicStrCleanUpSession()
{
	if (defaultClearerDSTR == NULL)
		defaultClearerDSTR = DSTR_CLEARER;
	vecClearPointersDSTR(&GC_dStr);
}

char* catStrings(const char* str1, ...)
{
	va_list strings;
	va_start(strings, str1);
	int size = 100;
	char* newS = (char*)str1;
	char* fullS = (char*)malloc(size);
	int position = 0; //start from the beginning

	while (newS != NULL)
	{
		int i = 0;
		int newSLen = strlen(newS);
		if (size <= (position + newSLen))
		{
			char* temp = (char*)realloc(fullS, (size += (position + newSLen + 16))); //offsetted memory allocation by an extra 16 bytes to reduce the number of realloc calls
			if (!temp)
			{
				free(fullS);
				fprintf(stderr, "\nERROR: reallocation for catStrings failed\n");
			}
			fullS = temp;
		}
		size -= newSLen;

		for (; newS[i] != '\0';)
		{
			fullS[position++] = newS[i++];
		}
		newS = (char*)va_arg(strings, const char*);
	}
	va_end(strings);
	fullS[position] = '\0';
	vecAddDSTR(&GC_dStr, fullS);
	if (fullS)
		return fullS;
	fprintf(stderr, "Variadic string concatenation has failed.\n");
	return NULL;
}


char* catStringsdp(const char** strings)
{
	char* fullS;
	int DPit = 0;

	int position = 0;
	int currentLen = 0;
	if (strings[0] != NULL)
		currentLen = strlen(strings[0]);
	else fprintf(stderr, "invalid buffer passed\n");
	fullS = (char*)malloc(currentLen + 1);
	while (strings != NULL && strings[DPit] != NULL)
	{
		const char* newS = (const char*)strings[DPit];
		for (int i = 0; newS[i] != '\0'; ++i)
		{
			if (fullS != NULL)
				fullS[position++] = newS[i];
			else fprintf(stderr, "Errors happened with allocation of the return string of catStrings\n");
		}
		int nextLen = strlen(strings[DPit++]);
		if ((position + nextLen) >= currentLen)
		{
			char* temp = (char*)realloc((void*)fullS, (currentLen += (position + nextLen))); //doing the same offset from before to not take up cpu cycles too much with realloc
			if (temp)
				fullS = temp;
		}


	}

	if (fullS)
	{
		fullS[position] = '\0';
		vecAddDSTR(&GC_dStr, fullS);
		return fullS;
	}
	fprintf(stderr, "Error occured with catStringsdp");
	return NULL;
}

typedef struct
{
	char* str;
	size_t begin;
	size_t end;
}coordString;
char* tokStr(const char* str, const char* delim)
{
	if (str == NULL || delim == NULL)
	{
		return NULL;
	}

	int len = strlen(str);
	char* fullStr = (char*)malloc(len + 1);
	if (fullStr == NULL)
	{
		return NULL;
	}

	int fullStrIndex = 0;
	for (int i = 0; str[i] != '\0'; ++i)
	{
		char unsigned isDelimiter = 0;


		for (int j = 0; delim[j] != '\0'; ++j) {
			if (str[i] == delim[j]) {
				isDelimiter = 1;
				break;
			}
		}


		if (!isDelimiter) {
			fullStr[fullStrIndex++] = str[i];
		}
	}

	fullStr[fullStrIndex] = '\0';
	vecAddDSTR(&GC_dStr, fullStr);
	return fullStr;
}
char* tokStrRep(const char* str, const char* delim, const char replace)
{
	if (str == NULL || delim == NULL)
	{
		return NULL;
	}

	int len = strlen(str);
	char* fullStr = (char*)malloc(len + 1);
	if (fullStr == NULL)
	{
		return NULL;
	}

	int fullStrIndex = 0;
	for (int i = 0; str[i] != '\0'; ++i)
	{
		char unsigned isDelim = 0;
		for (int j = 0; delim[j] != '\0'; ++j)
		{
			if (delim[j] == str[i])
			{
				isDelim = 1;
			}
		}
		if (isDelim)
			fullStr[fullStrIndex] = replace;
		else fullStr[fullStrIndex] = str[i];
		fullStrIndex++;
	}

	fullStr[fullStrIndex] = '\0';
	vecAddDSTR(&GC_dStr, fullStr);
	return fullStr;
}

coordString subStr(char* str, const char* kWord)
{
	int len = strlen(kWord);
	coordString s = { 0 };
	s.str = (char*)malloc(len + 1);
	s.str[len] = '\0';
	int k = 0;
	char unsigned begSet = 0;
	int strBeg = 0;
	int strEnd = 0;
	for (int i = 0; str[i] != '\0'; ++i)
	{

		if ((kWord[k]) == (str[i]))
		{
			if (s.begin == 0 && !begSet)
			{
				s.begin = i;
				strBeg = i;
				begSet = 1;
			}
			s.str[k] = str[i]; //point to the address of the start of that string.
			++k;

		}
		else
		{
			k = 0;
			s.begin = 0;
			begSet = 0;
		}
		if (k >= len)
		{
			s.end = i;
			strEnd = i;
			break; //found the first full occurence of the substring.
		}
	}
	vecAddDSTR(&GC_dStr, s.str);
	if (s.str != NULL)
		return s;
	else return s;
}
size_t getLength(coordString* cs)
{
	if (cs)
		return((cs->end + 1) - (cs->begin));

	return (size_t)NULL;
}

void printCString(coordString* cs)
{
	if (cs->str)
		printf("the substring: %s\n it's beginning in og string: %zu\n it's end in the og string: %zu\n", cs->str, cs->begin, cs->end);
	else
		printf("substring could not be found in the original string\n");
}
#ifdef __cplusplus
}
#endif
#endif 

