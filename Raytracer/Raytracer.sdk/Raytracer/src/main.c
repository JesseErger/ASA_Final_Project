//
//  main.c
//
//
//  Created by Valtteri Koskivuori on 12/02/15.
//
//

#include "includes.h"
#include "main.h"

#include "camera.h"
#include "errorhandler.h"
#include "filehandler.h"
#include "renderer.h"
#include "scene.h"
#include "ui.h"

int getFileSize(char *fileName);
int getSysCores();
void freeMem();

extern struct renderer mainRenderer;
extern struct poly *polygonArray;

void initRenderer(struct renderer *renderer) {
	renderer->renderBuffer = NULL;
	renderer->renderTiles = NULL;
	renderer->tileCount = 0;
	renderer->renderedTileCount = 0;
	renderer->activeThreads = 0;
	renderer->threadCount = getSysCores();
	renderer->mode = saveModeNormal;
	renderer->isRendering = false;
	renderer->renderPaused = false;
	renderer->avgTileTime = (time_t)1;
	renderer->timeSampleCount = 1;
	
	renderer->scene = (struct world*)calloc(1, sizeof(struct world));
}

/**
 Main entry point

 @param argc Argument count
 @param argv Arguments
 @return Error codes, 0 if exited normally
 */
int main(int argc, char *argv[]) {
	
	time_t start, stop;
	
	//Seed RNGs
	srand((int)time(NULL));
	srand48(time(NULL));
	
	//Initialize renderer
	initRenderer(&mainRenderer);
	
	char *fileName = NULL;
	//Build the scene
	if (argc == 2) {
		fileName = argv[1];
	} else {
		logHandler(sceneParseErrorNoPath);
	}
	
	//Build the scene
	switch (testBuild(&mainRenderer, fileName)) {
		case -1:
			logHandler(sceneBuildFailed);
			break;
		case -2:
			logHandler(sceneParseErrorMalloc);
			break;
		case 4:
			logHandler(sceneDebugEnabled);
			return 0;
			break;
		default:
			break;
	}
	
	//Check and set threadCount	
	if (mainRenderer.threadCount <= 0) {
		//mainRenderer.threadCount = getSysCores();
		mainRenderer.threadCount = 1;
	}
	
	//Quantize image into renderTiles
	quantizeImage();
	//Reorder those tiles
	reorderTiles(mainRenderer.tileOrder);
	//Compute the focal length for the camera
	computeFocalLength(&mainRenderer);
	
	//This is a timer to elapse how long a render takes per frame
	time(&start);
	
	//Create threads
	int t;
	
	//Alloc memory for pthread_create() args
	mainRenderer.renderThreadInfo = (struct threadInfo*)calloc(mainRenderer.threadCount, sizeof(struct threadInfo));
	if (mainRenderer.renderThreadInfo == NULL) {
		logHandler(threadMallocFailed);
		return -1;
	}
	
	//Verify sample count
	if (mainRenderer.sampleCount < 1) logHandler(renderErrorInvalidSampleCount);
	if (!mainRenderer.scene->camera->areaLights) mainRenderer.sampleCount = 1;
	
	printf("\nStarting C-ray renderer for frame %i\n\n", mainRenderer.scene->camera->currentFrame);
	
	//Print a useful warning to user if the defined tile size results in less renderThreads
	if (mainRenderer.tileCount < mainRenderer.threadCount) {
		printf("WARNING: Rendering with a less than optimal thread count due to large tile size!\n");
		printf("Reducing thread count from %i to ", mainRenderer.threadCount);
		mainRenderer.threadCount = mainRenderer.tileCount;
		printf("%i\n", mainRenderer.threadCount);
	}
	
	printf("Rendering at %i x %i\n", mainRenderer.image->size.width,mainRenderer.image->size.height);
	printf("Rendering with %i samples\n", mainRenderer.sampleCount);
	printf("Rendering with %d thread",mainRenderer.threadCount);
	if (mainRenderer.threadCount > 1) {
		printf("s\n");
	} else {
		printf("\n");
	}
	
	printf("Using %i light bounces\n", mainRenderer.scene->camera->bounces);
	printf("Raytracing...\n");
	
	//Allocate memory and create array of pixels for image data
	mainRenderer.image->data = (unsigned char*)calloc(3 * mainRenderer.image->size.width * mainRenderer.image->size.height, sizeof(unsigned char));
	
	//Allocate memory for render buffer
	//Render buffer is used to store accurate color values for the renderers' internal use
	mainRenderer.renderBuffer = (double*)calloc(3 * mainRenderer.image->size.width * mainRenderer.image->size.height, sizeof(double));
	
	//Allocate memory for render UI buffer
	//This buffer is used for storing UI stuff like currently rendering tile highlights
	mainRenderer.uiBuffer = (unsigned char*)calloc(4 * mainRenderer.image->size.width * mainRenderer.image->size.height, sizeof(unsigned char));
	
	
	if (!mainRenderer.image->data) logHandler(imageMallocFailed);
	
	mainRenderer.isRendering = true;
	mainRenderer.renderAborted = false;

	// pthread_attr_init(&mainRenderer.renderThreadAttributes);
	// pthread_attr_setdetachstate(&mainRenderer.renderThreadAttributes, PTHREAD_CREATE_JOINABLE);

	//Main loop (input)
	bool threadsHaveStarted = false;

	// Original thread based rendering
	/*
	while (mainRenderer.isRendering) {

		if (!threadsHaveStarted) {
			threadsHaveStarted = true;
			//Create render threads
			for (t = 0; t < mainRenderer.threadCount; t++) {
				mainRenderer.renderThreadInfo[t].thread_num = t;
				mainRenderer.renderThreadInfo[t].threadComplete = false;
				mainRenderer.activeThreads++;

				if (pthread_create(&mainRenderer.renderThreadInfo[t].thread_id, &mainRenderer.renderThreadAttributes, renderThread, &mainRenderer.renderThreadInfo[t])) {
					logHandler(threadCreateFailed);
					exit(-1);
				}
			}
			
			mainRenderer.renderThreadInfo->threadComplete = false;
			
			if (pthread_attr_destroy(&mainRenderer.renderThreadAttributes)) {
				logHandler(threadRemoveFailed);
			}
		}
		
		//Wait for render threads to finish (Render finished)
		for (t = 0; t < mainRenderer.threadCount; t++) {
			if (mainRenderer.renderThreadInfo[t].threadComplete && mainRenderer.renderThreadInfo[t].thread_num != -1) {
				mainRenderer.activeThreads--;
				mainRenderer.renderThreadInfo[t].thread_num = -1;
			}
			if (mainRenderer.activeThreads == 0 || mainRenderer.renderAborted) {
				mainRenderer.isRendering = false;
			}
		}
		sleepMSec(100);
	}
	
	//Make sure render threads are finished before continuing
	for (t = 0; t < mainRenderer.threadCount; t++) {
		if (pthread_join(mainRenderer.renderThreadInfo[t].thread_id, NULL)) {
			logHandler(threadFrozen);
		}
	}
	*/
	
	renderThread(&mainRenderer.renderThreadInfo[0]);

	time(&stop);
	printDuration(difftime(stop, start));
	
	//Write to file
	writeImage(mainRenderer.image);
	
	mainRenderer.scene->camera->currentFrame++;
	
	freeMem();
	
	printf("Render finished, exiting.\n");
	sleepMSec(10000);
	
	return 0;
}


/**
 Free dynamically allocated memory
 */
void freeMem() {
	//Free memory
	if (mainRenderer.image->data)
		free(mainRenderer.image->data);
	if (mainRenderer.renderThreadInfo)
		free(mainRenderer.renderThreadInfo);
	if (mainRenderer.renderBuffer)
		free(mainRenderer.renderBuffer);
	if (mainRenderer.uiBuffer)
		free(mainRenderer.uiBuffer);
	if (mainRenderer.scene->lights)
		free(mainRenderer.scene->lights);
	if (mainRenderer.scene->spheres)
		free(mainRenderer.scene->spheres);
	if (mainRenderer.scene->materials)
		free(mainRenderer.scene->materials);
	if (mainRenderer.renderTiles)
		free(mainRenderer.renderTiles);
	if (mainRenderer.scene)
		free(mainRenderer.scene);
	if (vertexArray)
		free(vertexArray);
	if (normalArray)
		free(normalArray);
	if (textureArray)
		free(textureArray);
	if (polygonArray)
		free(polygonArray);
}


/**
 Sleep for a given amount of milliseconds

 @param ms Milliseconds to sleep for
 */
void sleepMSec(int ms) {
	usleep(ms * 1000);
}


/**
 Get amount of logical processing cores on the system

 @return Amount of logical processing cores
 */
int getSysCores() {
	// TODO: Remove?
	return 1;
}
