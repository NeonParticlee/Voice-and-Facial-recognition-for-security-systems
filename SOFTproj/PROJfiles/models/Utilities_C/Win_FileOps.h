#ifndef WIN_FILE_OPS_H
#define WIN_FILE_OPS_H

/*my own implementations, need serious refinement*/

#include "ImageOps.h"

#include <Windows.h>


#ifdef __cplusplus
extern "C" {
#endif

	enableVec(char*, DirPaths)
	enableVec(char*, ImagePaths)


		void printDirPaths(VectorDirPaths* v)
	{
		printf("Directories Found: \n");
		for (int i = 0; i < v->currEnd; ++i)
		{
			printf("%d) %s\n", i + 1, v->data[i]);
		}
	}
	void printImagePaths(VectorImagePaths* v)
	{
		printf("Images Found: \n");
		for (int i = 0; i < v->currEnd; ++i)
		{
			printf("%d) %s\n", i + 1, v->data[i]);
		}
	}

#define DATA_FOLDER_PATH "D:\\DEVELOPMENT\\GENERAL C_CPP PROJECTS\\Face_Recognition\\Data"

#define GenerateDataFolder(name) catStrings(DATA_FOLDER_PATH, STRINGIFY(name), NULL)
	void mkDir(char* dirPath)
	{
		if (!dirPath) { fprintf(stderr, "the passed directory name was NULL\n"); return; }

		CreateDirectoryA((LPCSTR)dirPath, (LPSECURITY_ATTRIBUTES)NULL);

		DWORD err = GetLastError();
		if (err == ERROR_ALREADY_EXISTS)
		{
			fprintf(stderr, "The directory you specified to be created already exists.\n");
			return;
		}
		else if (err == ERROR_PATH_NOT_FOUND)
		{
			fprintf(stderr, "The directory path that was passed is malformed/not found\n");
		}
		else printf("allocated folder %s successfully\n", dirPath);
	}

	void mkMultipleDirs(char** dirPaths)
	{
		for (int i = 0; dirPaths[i] != NULL; ++i)
		{
			mkDir(dirPaths[i]);
		}
	}

	void OutputDirFiles(char* dirPath)
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		LARGE_INTEGER fileSize;

		if (dirPath != NULL)
		{
			char* allFiles = catStrings(dirPath, "\\*", NULL);
			hFind = FindFirstFileA(allFiles, &FindFileData);
		}

		else { fprintf(stderr, "directory passed was NULL\n"); return; }

		if (hFind == INVALID_HANDLE_VALUE)
		{
			return;
		}
		int counter = 0;
		do
		{

			if (counter < 2)
				continue;
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				printf("%s <DIR>\n", FindFileData.cFileName);
			}
			else
			{
				fileSize.LowPart = FindFileData.nFileSizeLow;
				fileSize.HighPart = FindFileData.nFileSizeHigh;
				printf("%s -- size: %ld bytes\n", FindFileData.cFileName, fileSize.QuadPart);
			}
			++counter;

		} while (FindNextFileA(hFind, &FindFileData) != 0);

		DWORD error = GetLastError();
		if (error != ERROR_NO_MORE_FILES)
		{
			fprintf(stderr, "an unexpected error happened while scrapping directory %s -- ERRNO: %du\n", dirPath, error);
		}
		FindClose(hFind);
	}

	VectorDirPaths getDirPaths(char* parentDirPath)
	{
		VectorDirPaths files = { 0 };

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;


		if (parentDirPath != NULL)
		{
			char* allFiles = catStrings(parentDirPath, "\\*", NULL);

			hFind = FindFirstFileA(allFiles, &FindFileData);

		}

		else { fprintf(stderr, "directory passed was NULL\n"); return files; }

		if (hFind == INVALID_HANDLE_VALUE)
		{
			return files; /*NULL files (directory is empty)*/
		}

		int counter = 0;
		do
		{
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && counter >= 2) /*primitive filter, in case the user passed a wild card to look for all file types in a certain directory*/
			{
				vecAddDirPaths(&files, catStrings(parentDirPath, "\\", FindFileData.cFileName, NULL)); /*SHARES THE STRINGS WITH THE STRING OPERATION GARBAGE COLLECTOR - no need to clean it up with clearPointers*/
			}
			++counter;
		} while (FindNextFileA(hFind, &FindFileData) != 0);

		DWORD error = GetLastError();
		if (error != ERROR_NO_MORE_FILES)
		{
			fprintf(stderr, "an unexpected error happened while scrapping directory %s -- ERRNO: %du\n", parentDirPath, error);
		}

		FindClose(hFind);

		return files;
	}

	VectorImagePaths getDirImagePaths(char* dirPath, char* format)
	{
		VectorImagePaths files = { 0 };

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		if (dirPath != NULL)
		{
			char* allFiles = catStrings(dirPath, "\\*", format, NULL);

			hFind = FindFirstFileA(allFiles, &FindFileData);
		}

		else { fprintf(stderr, "directory passed was NULL\n"); return files; }

		if (hFind == INVALID_HANDLE_VALUE)
		{
			return files; /*NULL files*/
		}

		do
		{
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) /*primitive filter, in case the user passed a wild card to look for all file types in a certain directory*/
				continue;

			vecAddImagePaths(&files, catStrings(dirPath, "\\", FindFileData.cFileName, NULL));
			/*SHARES THE STRINGS WITH THE STRING OPERATION GARBAGE COLLECTOR - no need to clean it up with clearPointers*/

		} while (FindNextFileA(hFind, &FindFileData) != 0);

		DWORD error = GetLastError();
		if (error != ERROR_NO_MORE_FILES)
		{
			fprintf(stderr, "an unexpected error happened while scrapping directory %s -- ERRNO: %du\n", dirPath, error);
		}

		FindClose(hFind);

		return files;
	}

	void fetchAllImagesinDirs(VectorDirPaths* dirPathsVec, VectorImagePaths* imgPathsVec, char* format)
	{
		if (!format || !dirPathsVec || !dirPathsVec->data)
		{
			fprintf(stderr, "data passed to the image fetcher was malformed.\n");
			return;
		}
		for (size_t i = 0; i < dirPathsVec->currEnd; ++i)
		{
			VectorImagePaths N = getDirImagePaths(dirPathsVec->data[i], format);

			vecReserveImagePaths(imgPathsVec, N.data, N.currEnd);
		}

	}

#ifdef __cplusplus
}
#endif

#endif