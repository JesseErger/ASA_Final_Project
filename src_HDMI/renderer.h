//
//  renderer.h
//  C-Ray
//
//  Created by Valtteri Koskivuori on 19/02/2017.
//  Copyright © 2017 Valtteri Koskivuori. All rights reserved.
//

#pragma once

struct world;
enum renderOrder;
struct outputImage;

/**
 Render tile, contains needed information for the renderer
 */
struct renderTile {
	int width;
	int height;
	//TODO: Consider position struct for these
	int startX, startY;
	int endX, endY;
	int completedSamples;
	bool isRendering;
	int tileNum;
};

enum renderOrder {
	renderOrderTopToBottom = 0,
	renderOrderFromMiddle,
	renderOrderToMiddle,
	renderOrderNormal,
	renderOrderRandom
};

/**
 Main renderer. Stores needed information to keep track of render status,
 as well as information needed for the rendering routines.
 */
struct renderer {
	struct threadInfo *renderThreadInfo; //Info about threads

	struct world *scene; //Scene to render
	struct outputImage *image; //Output image
	struct renderTile *renderTiles; //Array of renderTiles to render
	int tileCount; //Total amount of render tiles
	enum fileMode mode;
	int renderedTileCount; //Completed render tiles
	double *renderBuffer;  //Double-precision buffer for multisampling
	int activeThreads; //Amount of threads currently rendering
	bool isRendering;
	bool smoothShading;//Unused
	//time_t avgTileTime;//Used for render duration estimation
	int timeSampleCount;//Used for render duration estimation, amount of time samples captured
	
	//Prefs
	int sampleCount;
	bool antialiasing;
	bool newRenderer;
	int tileWidth;
	int tileHeight;
};


 //Renderer
double renderThread();

 /*
//Renderer
#ifdef WINDOWS
DWORD WINAPI renderThread(LPVOID arg);
#else
void *renderThread(void *arg);
#endif
*/
void quantizeImage();

