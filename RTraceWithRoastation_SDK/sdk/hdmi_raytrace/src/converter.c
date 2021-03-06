//
//  converter.c
//  C-Ray
//
//  Created by Valtteri Koskivuori on 18/08/2017.
//  Copyright © 2017 Valtteri Koskivuori. All rights reserved.
//

#include "includes.h"
#include "converter.h"

#include "obj_parser.h"

/**
 Convert a given OBJ loader vector into a c-ray vector
 
 @param vec OBJ loader vector
 @return c-ray vector
 */
struct vector vectorFromObj(obj_vector *vec) {
	struct vector vector;
	vector.x = vec->e[0];
	vector.y = vec->e[1];
	vector.z = vec->e[2];
	vector.isTransformed = false;
	return vector;
}
/*
 Convert a given OBJ loader material into a c-ray material
 
 */
struct material materialFromObj(obj_material *mat) {
	struct material newMat;
	newMat.diffuse.red   = mat->diff[0];
	newMat.diffuse.green = mat->diff[1];
	newMat.diffuse.blue  = mat->diff[2];
	newMat.diffuse.alpha = 0;
	newMat.ambient.red   = mat->amb[0];
	newMat.ambient.green = mat->amb[1];
	newMat.ambient.blue  = mat->amb[2];
	newMat.ambient.alpha = 0;
	newMat.specular.red  = mat->spec[0];
	newMat.specular.green= mat->spec[1];
	newMat.specular.blue = mat->spec[2];
	newMat.specular.alpha= 0;
	newMat.reflectivity  = mat->reflect;
	newMat.refractivity  = mat->refract;
	newMat.IOR           = mat->refract_index;
	newMat.glossiness    = mat->glossy;
	newMat.transparency  = mat->trans;
	newMat.sharpness     = 0;
	return newMat;
}
