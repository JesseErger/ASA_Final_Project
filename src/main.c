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

int getFileSize(char *fileName);
void freeMem();

extern struct renderer mainRenderer;

void initRenderer(struct renderer *renderer) {
	renderer->renderBuffer = NULL;
	renderer->renderTiles = NULL;
	renderer->tileCount = 0;
	renderer->renderedTileCount = 0;
	renderer->mode = saveModeNormal;
	renderer->isRendering = false;
	renderer->renderPaused = false;
	//renderer->avgTileTime = (time_t)1;
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

	//time_t start, stop;

	srand(1);

	//Initialize renderer
	initRenderer(&mainRenderer);

	//Build the scene
	switch (testBuild(&mainRenderer)) {
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
	//Quantize image into renderTiles
	quantizeImage();
	//Compute the focal length for the camera
	computeFocalLength(&mainRenderer);
	//This is a timer to elapse how long a render takes per frame
	//time(&start);

	//Verify sample count
	//if (mainRenderer.sampleCount < 1) logHandler(renderErrorInvalidSampleCount);
	if (!mainRenderer.scene->camera->areaLights) mainRenderer.sampleCount = 1;

	printf("\nStarting C-ray renderer for frame %i\n\n", mainRenderer.scene->camera->currentFrame);


	printf("Rendering at %i x %i\n", mainRenderer.image->size.width, mainRenderer.image->size.height);
	printf("Rendering with %i samples\n", mainRenderer.sampleCount);
	printf("Using %i light bounces\n", mainRenderer.scene->camera->bounces);
	printf("Raytracing...\n");

	//Allocate memory and create array of pixels for image data
	//mainRenderer.image->data = (unsigned char*)calloc(3 * mainRenderer.image->size.width * mainRenderer.image->size.height, sizeof(unsigned char));

	//Allocate memory for render buffer
	//Render buffer is used to store accurate color values for the renderers' internal use
	mainRenderer.renderBuffer = (double*)calloc(4 * mainRenderer.image->size.width * mainRenderer.image->size.height, sizeof(double));

	//Allocate memory for render UI buffer
	//This buffer is used for storing UI stuff like currently rendering tile highlights
	//mainRenderer.uiBuffer = (unsigned char*)calloc(4 * mainRenderer.image->size.width * mainRenderer.image->size.height, sizeof(unsigned char));



	//if (!mainRenderer.image->data) logHandler(imageMallocFailed);

	mainRenderer.isRendering = true;
	mainRenderer.renderAborted = false;
	//Main loop (input)
	renderThread();
	//time(&stop);
	//Write to file
	writeImage(mainRenderer.image);

	mainRenderer.scene->camera->currentFrame++;

	freeMem();

	printf("Render finished, exiting.\n");

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
}


/**
Sleep for a given amount of milliseconds

@param ms Milliseconds to sleep for
*/
void sleepMSec(int ms) {
#ifdef WINDOWS
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}


/**
Get amount of logical processing cores on the system

@return Amount of logical processing cores
*/
