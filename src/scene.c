//
//  scene.c
//  C-Ray
//
//  Created by Valtteri Koskivuori on 28/02/15.
//  Copyright (c) 2015 Valtteri Koskivuori. All rights reserved.
//

#include "includes.h"
#include "scene.h"

#include "camera.h"
#include "light.h"
#include "obj_parser.h"
#include "filehandler.h"
#include "renderer.h"

#define TOKEN_DEBUG_ENABLED false

char *trimSpaces(char *inputLine);

/**
 Extract the filename from a given file path

 @param input File path to be processed
 @return Filename string, including file type extension
 */
char *getFileName(char *input) {
	char *fn;

	/* handle trailing '/' e.g.
	 input == "/home/me/myprogram/" */
	if (input[(strlen(input) - 1)] == '/')
		input[(strlen(input) - 1)] = '\0';

	(fn = strrchr(input, '/')) ? ++fn : (fn = input);

	return fn;
}

//FIXME: change + 1 to ++scene->someCount and just pass the count to array access
//In the future, maybe just pass a list and size and copy at once to save time (large counts)
void addSphere(struct world *scene, struct sphere newSphere) {
	scene->spheres = (struct sphere*)realloc(scene->spheres, (scene->sphereCount + 1) * sizeof(struct sphere));
	scene->spheres[scene->sphereCount++] = newSphere;
}

void addLight(struct world *scene, struct light newLight) {
	scene->lights = (struct light*)realloc(scene->lights, (scene->lightCount + 1) * sizeof(struct light));
	scene->lights[scene->lightCount++] = newLight;
}




//FIXME: Move this to transforms.c
void addCamTransform(struct camera *cam, struct matrixTransform transform) {
	if (cam->transformCount == 0) {
		cam->transforms = (struct matrixTransform*)calloc(1, sizeof(struct matrixTransform));
	} else {
		cam->transforms = (struct matrixTransform*)realloc(cam->transforms, (cam->transformCount + 1) * sizeof(struct matrixTransform));
	}
	
	cam->transforms[cam->transformCount] = transform;
	cam->transformCount++;
}

int testBuild(struct renderer *r) {
	printf("Starting SceneBuilder V0.5\n\n");
	
	//MATERIALS
	/*
	addMaterial(r->scene, newMaterialFull(colorWithValues(0.1, 0.05, 0.05, 0.0),
										colorWithValues(0.6, 0.1, 0.1, 0.0),
										colorWithValues(1, 0.2, 0.2, 0.0), .2, .1, 0, 0, 0, 20.)); //Matte red



	addMaterial(r->scene, newMaterial(colorWithValues(0.1, 0.1, 0.1, 0.0), 0.0)); //Matte green
	addMaterial(r->scene, newMaterial(colorWithValues(0.1, 0.1, 0.2, 0.0), 0.0)); //Matte blue
	addMaterial(r->scene, newMaterial(colorWithValues(0.8, 0.8, 0.8, 0.0), 0.0));
	addMaterial(r->scene, newMaterial(colorWithValues(0.0, 0.5, 1.0, 0.0), 1.0)); //0.517647
	addMaterial(r->scene, newMaterial(colorWithValues(0.3, 0.3, 0.3, 0.0), 1.0));
	addMaterial(r->scene, newMaterial(colorWithValues(0.3, 0.0, 0.0, 0.0), 1.0));
	addMaterial(r->scene, newMaterial(colorWithValues(0.0, 0.3, 0.0, 0.0), 1.0));
	addMaterial(r->scene, newMaterial(colorWithValues(0.0, 0.0, 0.3, 0.0), 0.0));
	addMaterial(r->scene, newMaterial(colorWithValues(0.9, 0.9, 0.9, 0.0), 0.0));
	addMaterial(r->scene, newMaterial(colorWithValues(1.0, 0.0, 0.0, 0.0), 0.0));
	*/
	//Output image prefs
	r->image = (struct outputImage*)calloc(1, sizeof(struct outputImage));
	r->image->filePath = "output/";
	r->image->fileName = "rendered";
	r->image->count = 0;
	r->image->size.width = 1280;
	r->image->size.height = 800;
	r->image->fileType = png;

	//Renderer prefs
	r->threadCount = 0; //Override, 0 defaults to physical core count
	r->sampleCount = 25;
	r->antialiasing = true;
	r->newRenderer = true; //New, recursive rayTracing algorighm (buggy!)
	r->tileWidth = 128;
	r->tileHeight = 128;
	
	//Camera prefs
	//TODO: Move camera to renderer
	r->scene->camera = (struct camera*)calloc(1, sizeof(struct camera));
	initCamera(r->scene->camera);
	r->scene->camera->isFullScreen = false;
	r->scene->camera->isBorderless = false;
	r->scene->camera-> windowScale = 1.0;
	
	r->scene->camera->         FOV = 80.0;
	r->scene->camera->    aperture = 0.0;
	r->scene->camera->    contrast = 0.5;
	r->scene->camera->bounces = 3;
	r->scene->camera->areaLights = true;
	
	//comment this block, and uncomment the next block below to toggle the detailed view of the lighting bug
	addCamTransform(r->scene->camera, newTransformTranslate(970, 480, 600)); //Set pos here
	addCamTransform(r->scene->camera, newTransformRotateX(21));//And add as many rotations as you want!
	addCamTransform(r->scene->camera, newTransformRotateZ(9)); //Don't scale or translate!
	transformCameraIntoView(r->scene->camera);
	
	//Comment above block, and uncomment this to toggle the detailed view
	/*addCamTransform(r->scene->camera, newTransformTranslate(750, 550, 1500)); //Set pos here
	addCamTransform(r->scene->camera, newTransformRotateX(21));//And add as many rotations as you want!
	addCamTransform(r->scene->camera, newTransformRotateY(90));*/
	
	r->scene->ambientColor = (struct color*)calloc(1, sizeof(struct color));
	r->scene->ambientColor->  red = 0.4;
	r->scene->ambientColor->green = 0.6;
	r->scene->ambientColor-> blue = 0.6;
	
	//LIGHTS
	
	addLight(r->scene, newLight(vectorWithPos(970, 450, 500), 50, colorWithValues(2, 2, 4, 0)));
	addLight(r->scene, newLight(vectorWithPos(1210, 450,1050), 100, colorWithValues(5, 0, 0, 0)));

	addLight(r->scene, newLight(vectorWithPos(1160, 400, 0),    13, colorWithValues(0.2, 0.2, 0.2, 0.0)));
	addLight(r->scene, newLight(vectorWithPos(760 , 500, 0),    42, colorWithValues(0.2, 0.2, 0.2, 0.0)));
	//addLight(r->scene, newLight(vectorWithPos(640 , 350, 600), 200, colorWithValues(6.0, 0.0, 0.0, 0.0)));
	//addLight(r->scene, newLight(vectorWithPos(940 , 350, 600), 200, colorWithValues(0.0, 6.0, 0.0, 0.0)));
	//addLight(r->scene, newLight(vectorWithPos(1240, 350, 600), 200, colorWithValues(0.0, 0.0, 6.0, 0.0)));
	
	//addSphere(r->scene, newSphere(vectorWithPos(650, 450, 1650), 150, 5));
	//addSphere(r->scene, newSphere(vectorWithPos(950, 350, 1500), 50, 6));
	//addSphere(r->scene, newSphere(vectorWithPos(1100, 350, 1500), 50, 8));
return 0;
}


