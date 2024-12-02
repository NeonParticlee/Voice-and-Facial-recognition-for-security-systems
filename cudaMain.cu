
#include "CNN.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "Win_FileOps.h"

#ifdef __cplusplus
}
#define sleepEveryNFrames 60 /*will do a modulo operation between a loop incrementer/global counter by this amount and if it equals zero we will invoke Sleep(sleepTime)*/
#define sleepTime 1

void cleanUpRoutine()
{
	dynamicStrCleanUpSession();
	vecClearPointersImgData(&imgData);
	vecClearPointersnormImageData(&normalizedImageDataVec);
	//FileOpsCleanUpSession(); no need right now as no fileops.h operations were used yet
}

int main()
{	
	/*CPU-*/
	defaultPrinterImagePaths = printImagePaths;
	defaultPrinterDirPaths = printDirPaths;
	defaultClearerImgData = clearImgData;
	defaultClearernormImageData = clearNormData;
	
		/*adding new face data folders (each person has their own unique folder in the database)*/
		size_t n = 0;
		char trailing = 0;
		printf("Number of people to add newly: ");
		scanf_s("%zu", &n);
		scanf("%c", &trailing);

		for (size_t i = 0; i < n; ++i)
		{
			char b[MAXPATH];
			printf("enter label: ");
			fgets(b, MAXPATH, stdin);
			for (size_t i = 0; i < MAXPATH; ++i)
			{
				if (b[i] == '\n')
				{
					b[i] = '\0';
					break;
				}
			}
			mkDir(catStrings(DATA_FOLDER_PATH, "\\", b, NULL));
		}
		cleanUpRoutine();

		VectorDirPaths dirs = getDirPaths((char*)DATA_FOLDER_PATH);
		vecPrintDirPaths(&dirs);

		
		VectorImagePaths img = { 0 };
		fetchAllImagesinDirs(&dirs, &img, (char*)".jpg");
		vecPrintImagePaths(&img);
		
		/*---------------------------------IMPORTANT---------------------------------------*
		checkpoint - our file fetching and path construction is done, we can now focus on handling loading the image data using STB to finally process them into the model*/
	/*	for (size_t i = 0; i < img.currEnd; ++i) - train on all images in the whole data folder, too complex right now
		{
			stbImage Image = { 0 };
			loadImage(img.data[i], &Image);
		}
		*/
		stbImage image = { 0 };
		loadImage(img.data[0], &image);
		printf("the path we're working with: %s\n", img.data[0]);
		
		processed_stbImage nImage = { 0 };
		initProcessedImageParams(&nImage, RESIZE_DIMENSIONS, 1);
		preProcessImage(&image, &nImage);
		size_t imgSize = nImage.height * nImage.width * nImage.channels;
		
		/*training session - at the end of each one, various vectors should be reset
		just call cleanUpRoutine() at the end of each session - it takes care of the normalized and non normalized pixel image data along with the dynamicStr function's dynamically allocated strings
		since our file manipulation and image manipulation libraries require a ton of dynamicness and store copies of the heap addresses in those vectors
		other heap address containers like image paths and dirpaths are not exactly critical so they are only cleaned up at the very end of the program
		the fileops header meanwhile is not used yet and will be seldom used - it's just needed to update the models weights at the end of the program or constantly after the end of each full training session on an image/image batch or a testing session if new weight or label data need be stored
		if so just take away the comment of its clearer from the cleanUpRoutine function
		*/







		/*final cleanups; leave at the very end of the code - this block can also be copied to other parts of the code but that won't 
		be really necessary since our path vectors share dynamic memory with utility lib vectors so it is just essential to call the global cleanup function at the end of each 
		major session*/
		/*CPU - end of whole session clean ups*/
		vecClearImagePaths(&img);
		vecClearDirPaths(&dirs);
		cleanUpRoutine();
	return 0;
}


#endif