//
//  scene.h
//  C-Ray
//
//  Created by Valtteri Koskivuori on 28/02/15.
//  Copyright (c) 2015 Valtteri Koskivuori. All rights reserved.
//

#pragma once


struct light;
struct material;
struct sphere;
struct camera;
struct matrixTransform;
struct renderer;

//World
struct world {
	
	//Ambient background color.
	struct color *ambientColor;
	
	//Lights
	struct light *lights;
	int lightCount;
	
	//Materials
	struct material *materials;
	int materialCount;
	
	//Spheres
	struct sphere *spheres;
	int sphereCount;
	
	//Currently only one camera supported
	struct camera *camera;
	int cameraCount;
	
};

int testBuild(struct renderer *renderer);
