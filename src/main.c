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
int getSysCores();
void freeMem();

extern struct renderer mainRenderer;
extern struct poly *polygonArray;

void initRenderer(struct renderer *renderer) {
	renderer->renderBuffer = NULL;
	renderer->renderTiles = NULL;
	renderer->tileCount = 0;
	renderer->renderedTileCount = 0;
	renderer->mode = saveModeNormal;
	renderer->isRendering = false;
	renderer->renderPaused = false;
	renderer->avgTileTime = (time_t)1;
	renderer->timeSampleCount = 1;
	
	renderer->scene = (struct world*)calloc(1, sizeof(struct world));
}

int main(int argc, char *argv[]) {
	//Seed RNGs
	// TODO: pick a random seed
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

	//Check and set threadCount
	if (mainRenderer.threadCount <= 0) {
		mainRenderer.threadCount = 1;
	}

	//Quantize image into renderTiles
	quantizeImage();
	//Compute the focal length for the camera
	computeFocalLength(&mainRenderer);
	
	//Verify sample count
	if (mainRenderer.sampleCount < 1) logHandler(renderErrorInvalidSampleCount);
	if (!mainRenderer.scene->camera->areaLights) mainRenderer.sampleCount = 1;
	
	printf("\nStarting C-ray renderer for frame %i\n\n", mainRenderer.scene->camera->currentFrame);
	
	
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
	//mainRenderer.image->data = (unsigned char*)calloc(3 * mainRenderer.image->size.width * mainRenderer.image->size.height, sizeof(unsigned char));
	
	//Allocate memory for render buffer
	//Render buffer is used to store accurate color values for the renderers' internal use
	mainRenderer.renderBuffer = (double*)calloc(3 * mainRenderer.image->size.width * mainRenderer.image->size.height, sizeof(double));

	mainRenderer.isRendering = true;
	mainRenderer.renderAborted = false;

	renderThread(&mainRenderer.renderThreadInfo[0]);
	
	//Write to file
	//writeImage(mainRenderer.image);
	// TODO: add some printing code
	
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
	usleep(ms * 1000);
}
