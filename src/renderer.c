//
//  renderer.c
//  C-Ray
//
//  Created by Valtteri Koskivuori on 19/02/2017.
//  Copyright © 2017 Valtteri Koskivuori. All rights reserved.
//

#include "includes.h"
#include "renderer.h"
#include "camera.h"
#include "light.h"
#include "scene.h"
#include "raytrace.h"
#include "filehandler.h"
#include "main.h"

/*
 * Global renderer
 */
struct renderer mainRenderer;


/**
 Gets the next tile from renderTiles in mainRenderer
 
 @return A renderTile to be rendered
 */
struct renderTile getTile() {
	struct renderTile tile;
	memset(&tile, 0, sizeof(tile));
	tile.tileNum = -1;
	if (mainRenderer.renderedTileCount < mainRenderer.tileCount) {
		tile = mainRenderer.renderTiles[mainRenderer.renderedTileCount];
		mainRenderer.renderTiles[mainRenderer.renderedTileCount].isRendering = true;
		tile.tileNum = mainRenderer.renderedTileCount++;
	}
	return tile;
}

/**
 Create tiles from render plane, and add those to mainRenderer
 
 @param scene scene object
 */
void quantizeImage() {
	printf("Quantizing render plane...\n");
	
	//Sanity check on tilesizes
	if (mainRenderer.tileWidth >= mainRenderer.image->size.width) mainRenderer.tileWidth = mainRenderer.image->size.width;
	if (mainRenderer.tileHeight >= mainRenderer.image->size.height) mainRenderer.tileHeight = mainRenderer.image->size.height;
	if (mainRenderer.tileWidth <= 0) mainRenderer.tileWidth = 1;
	if (mainRenderer.tileHeight <= 0) mainRenderer.tileHeight = 1;
	
	int tilesX = mainRenderer.image->size.width / mainRenderer.tileWidth;
	int tilesY = mainRenderer.image->size.height / mainRenderer.tileHeight;
	
	tilesX = (mainRenderer.image->size.width % mainRenderer.tileWidth) != 0 ? tilesX + 1: tilesX;
	tilesY = (mainRenderer.image->size.height % mainRenderer.tileHeight) != 0 ? tilesY + 1: tilesY;
	
	mainRenderer.renderTiles = (struct renderTile*)calloc(tilesX*tilesY, sizeof(struct renderTile));
	if (mainRenderer.renderTiles == NULL) {
		printf("Failed to allocate renderTiles array!\n");
		abort();
	}
	
	for (int y = 0; y < tilesY; y++) {
		for (int x = 0; x < tilesX; x++) {
			struct renderTile *tile = &mainRenderer.renderTiles[x + y*tilesX];
			tile->width  = mainRenderer.tileWidth;
			tile->height = mainRenderer.tileHeight;
			
			tile->startX = x       * mainRenderer.tileWidth;
			tile->endX   = (x + 1) * mainRenderer.tileWidth;
			
			tile->startY = y       * mainRenderer.tileHeight;
			tile->endY   = (y + 1) * mainRenderer.tileHeight;
			
			tile->endX = min((x + 1) * mainRenderer.tileWidth, mainRenderer.image->size.width);
			tile->endY = min((y + 1) * mainRenderer.tileHeight, mainRenderer.image->size.height);
			
			//Samples have to start at 1, so the running average works
			tile->completedSamples = 1;
			tile->isRendering = false;
			tile->tileNum = mainRenderer.tileCount;
			
			mainRenderer.tileCount++;
		}
	}
	printf("Quantized image into %i tiles. (%ix%i)", (tilesX*tilesY), tilesX, tilesY);
}


/**
 Reorder renderTiles to start from top
 */
void reorderTopToBottom() {
	int endIndex = mainRenderer.tileCount - 1;
	
	struct renderTile *tempArray = (struct renderTile*)calloc(mainRenderer.tileCount, sizeof(struct renderTile));
	
	for (int i = 0; i < mainRenderer.tileCount; i++) {
		tempArray[i] = mainRenderer.renderTiles[endIndex--];
	}
	
	free(mainRenderer.renderTiles);
	mainRenderer.renderTiles = tempArray;
}

unsigned int rand_interval(unsigned int min, unsigned int max) {
	int r;
	const unsigned int range = 1 + max - min;
	const unsigned int buckets = RAND_MAX / range;
	const unsigned int limit = buckets * range;
	
	/* Create equal size buckets all in a row, then fire randomly towards
	 * the buckets until you land in one of them. All buckets are equally
	 * likely. If you land off the end of the line of buckets, try again. */
	do
	{
		r = rand();
	} while (r >= limit);
	
	return min + (r / buckets);
}

/**
 Gets a pixel from the render buffer
 
 @param x X coordinate of pixel
 @param y Y coordinate of pixel
 @return A color object, with full color precision intact (double)
 */
struct color getPixel(int x, int y) {
	struct color output = {0.0, 0.0, 0.0, 0.0};
	output.red = mainRenderer.renderBuffer[(x + (mainRenderer.image->size.height - y) * mainRenderer.image->size.width)*3 + 0];
	output.green = mainRenderer.renderBuffer[(x + (mainRenderer.image->size.height - y) * mainRenderer.image->size.width)*3 + 1];
	output.blue = mainRenderer.renderBuffer[(x + (mainRenderer.image->size.height - y) * mainRenderer.image->size.width)*3 + 2];
	output.alpha = 1.0;
	return output;
}

/**
 A render thread
 
 @param arg Thread information (see threadInfo struct)
 @return Exits when thread is done
 */
void *renderThread(void *arg) {
	struct lightRay incidentRay;
	//First time setup for each thread
	struct renderTile tile = getTile();
	print("Made it here");
	while (tile.tileNum != -1) {
		while (tile.completedSamples < mainRenderer.sampleCount+1 && mainRenderer.isRendering) {
			for (int y = tile.endY; y > tile.startY; y--) {
				for (int x = tile.startX; x < tile.endX; x++) {

					int height = mainRenderer.image->size.height;
					int width = mainRenderer.image->size.width;

					double fracX = (double)x;
					double fracY = (double)y;

					//A cheap 'antialiasing' of sorts. The more samples, the better this works
					if (mainRenderer.antialiasing) {
						fracX = getRandomDouble(fracX - 0.25, fracX + 0.25);
						fracY = getRandomDouble(fracY - 0.25, fracY + 0.25);
					}

					//Set up the light ray to be casted. direction is pointing towards the X,Y coordinate on the
					//imaginary plane in front of the origin. startPos is just the camera position.
					struct vector direction = {(fracX - 0.5 * mainRenderer.image->size.width)
												/ mainRenderer.scene->camera->focalLength,
											   (fracY - 0.5 * mainRenderer.image->size.height)
												/ mainRenderer.scene->camera->focalLength,
												1.0,
												false};

					//Normalize direction
					direction = normalizeVector(&direction);
					struct vector startPos = mainRenderer.scene->camera->pos;
					struct vector left = mainRenderer.scene->camera->left;
					struct vector up = mainRenderer.scene->camera->up;

					//Run camera tranforms on direction vector
					transformCameraView(mainRenderer.scene->camera, &direction);

					//Now handle aperture
					//FIXME: This is a 'square' aperture
					double aperture = mainRenderer.scene->camera->aperture;
					if (aperture <= 0.0) {
						incidentRay.start = startPos;
					} else {
						double randY = getRandomDouble(-aperture, aperture);
						double randX = getRandomDouble(-aperture, aperture);
						
						struct vector upTemp = vectorScale(randY, &up);
						struct vector temp = addVectors(&startPos, &upTemp);
						struct vector leftTemp = vectorScale(randX, &left);
						struct vector randomStart = addVectors(&temp, &leftTemp);
						
						incidentRay.start = randomStart;
					}

					incidentRay.direction = direction;
					incidentRay.rayType = rayTypeIncident;
					incidentRay.remainingInteractions = mainRenderer.scene->camera->bounces;
					incidentRay.currentMedium.IOR = AIR_IOR;

					//For multi-sample rendering, we keep a running average of color values for each pixel
					//The next block of code does this

					//Get previous color value from render buffer
					struct color output = getPixel(x, y);
					struct color sample = {0.0,0.0,0.0,0.0};

					//Get new sample (raytracing is initiated here)
					if (mainRenderer.newRenderer) {
						sample = newTrace(&incidentRay, mainRenderer.scene);
					} else {
						sample = rayTrace(&incidentRay, mainRenderer.scene);
					}

					//And process the running average
					output.red = output.red * (tile.completedSamples - 1);
					output.green = output.green * (tile.completedSamples - 1);
					output.blue = output.blue * (tile.completedSamples - 1);

					output = addColors(&output, &sample);

					output.red = output.red / tile.completedSamples;
					output.green = output.green / tile.completedSamples;
					output.blue = output.blue / tile.completedSamples;

					//Store render buffer
					mainRenderer.renderBuffer[(x + (height - y)*width)*3 + 0] = output.red;
					mainRenderer.renderBuffer[(x + (height - y)*width)*3 + 1] = output.green;
					mainRenderer.renderBuffer[(x + (height - y)*width)*3 + 2] = output.blue;

					// print the colors
					// printf("R: %f, G: %f, B: %f\r\n", output.red, output.green, output.blue);
				}
			}
			tile.completedSamples++;

			printf("sample %d done\n", tile.completedSamples);

			//Pause rendering when bool is set
			while (mainRenderer.renderPaused) {
				sleepMSec(100);
				if (!mainRenderer.renderPaused) break;
			}
		}
		//Tile has finished rendering, get a new one and start rendering it.
		mainRenderer.renderTiles[tile.tileNum].isRendering = false;
		tile = getTile();

		printf("Tile %d done\n", mainRenderer.renderedTileCount);
	}
}
