/*Unit test for vector operations of cutil*/
#include "cutil.h"

#define maxBuff 20'000i32

char* formatIn(char* in)
{
	if (in == NULL)
		return;
	for (int i = 0; in[i]!='\0'; ++i)
	{
		if (in[i] == '\0') continue;
		
		else if (in[i] == ';')
		{
			if (in[i + 1] == ' ') { //only put \n in place of a whitespace
				in[i + 1] = '\n';
			}
			else printf("---------------------------WTF\n");
		}
	}
	return in;
}

/******************HERE******************/
void show(void)
{
	char* b = FileOperation("iFILE.txt", "rb", NULL);
	b = formatIn(b);
	printf("Output of the preprocessor: \n\n\n%s", b);
}
void process(char* filepath, char* filePath)
{
	char* fileTBP = FileOperation(filepath, "rb", NULL);
	fileTBP = tokStrRep(fileTBP, "\\", ' ');
	FileOperation(filePath, "wb", fileTBP);
}

typedef struct
{
	int a;
	int b;
	char c;
} test;
void print(test* structP)
{
	printf("%d, %d, %c", structP->a, structP->b, structP->c);
}

enableVec(float, F);
#if 1
enableVec(unsigned char, C);


/************************************/

enableVec(int, I);
enableVec(char unsigned*, str);

enableStructVec(test, st);


void printing(VectorC vec)
{
	for (int i = 0; i < getVecEndC(&vec)-1; ++i)
	{
		printf("%c", vec.data[i]);
	}
}
void printStrings(Vectorstr str)
{
	for (int i = 0; i < getVecEndstr(&str); ++i)
	{
		printf("strings: %s\n", str.data[i]);
	}
}




/***********************Good example of a fully compatible and memory safe clearer with our dynamic library, 
it shouldn't clear the outer data pointer itself but only deal with inner pointers, since the clearing function itself handles the outer pointer*********************/
void cl(Vectorstr* a)
{

	for (int i = 0; i < a->currEnd; ++i)
	{
		free(a->data[i]);
	}
	
}
char unsigned d;
callbackPointerClearerstr clearer = cl;
callbackPrinterIterativest;
void pr2(Vectorst* vec)
{
	for (int i = 0; i < vec->currEnd; ++i)
	{
		printf("Struct %d data: a: %d, b: %d, c: %c\n", i, vec->data[i].a, vec->data[i].b, vec->data[i].c);
	}
}
void pri2(test stru)
{
	
		printf("data: a: %d, b: %d, c: %c", stru.a, stru.b, stru.c);
}

#endif

int main()
{
	initDefaultFunctionsst(NULL, pr2, pri2);
#if 1
	Vectorstr str = { 0 };
	VectorC c = { 0 };
	
	Vectorstr str2 = { 0 };
	Vectorst d = { 0 }; /*monumental achievement.*/
	while (1) { /*checking for memory leaks*/
		
		char* s1 = malloc(5);
		strcpy(s1, "Omar");
		char* s2 = malloc(8);
		strcpy(s2, "Mohamed");
		char* s3 = malloc(8);
		strcpy(s3, "Hussein");
		d = vecCreatest((test[]) { { 1, 2, '3' }, { 3, 5, '6' }, { 50, 30, '1' }, { 1000, 3000, '5' } }, 4);
		vecPrintst(&d);
		vecPrintIterativest(&d);
		vecReservest(&d, (test[]) { { 50, 60, '6' }, { 20, 30, '4' } }, 2);
		vecReversest(&d);
		vecPrintst(&d);
		vecDeletest(&d, 3);
		vecPrintst(&d);
		vecClearst(&d);
		vecPrintst(&d);
		printf("CUTOFF\n");
		char* b[] = { s1, s2, s3 };

		vecReservestr(&str, b, arrsize(b));
		vecReversestr(&str);
		vecPrintCustomstr(&str, printStrings);
		vecClearPointersCustomstr(&str, clearer);
		c = vecCreateC("hello wehld", 12);
		vecPrintCustomC(&c, printing);

		vec_printC(&c);
		vecAddC(&c, 'c');
		vecPrintLengthsC(&c);
		vecPrintCustomC(&c, printing);
		vecOverrideC(&c, "Hello Mohammed", 15);
		vecPrintCustomC(&c, printing);
		vecPrintLengthsC(&c);
		vecAddC(&c, 'g');
		vecSortC(&c, 0);
		vecPrintCustomC(&c, printing);

		vecReserveC(&c, "yes yes", 8);
		vecPrintCustomC(&c, printing);
		vecClearC(&c);
		vecReserveC(&c, setSTRING("YEEEEEE"));
		vecPrintCustomC(&c, printing);
		vecReserveC(&c, setSTRING("WOHOO"));
		vecPrintCustomC(&c, printing);
		vecClearC(&c);

#endif

	}


	
	return 0;
}
