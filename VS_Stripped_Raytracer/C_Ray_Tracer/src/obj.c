//
//  obj.c
//  C-Ray
//
//  Created by Valtteri Koskivuori on 27/04/2017.
//  Copyright © 2017 Valtteri Koskivuori. All rights reserved.
//

#include "includes.h"
#include "obj.h"

#include "poly.h"
#include "bbox.h"


void addTransform(struct crayOBJ *obj, struct matrixTransform transform) {
	if (obj->transformCount == 0) {
		obj->transforms = (struct matrixTransform*)calloc(1, sizeof(struct matrixTransform));
	} else {
		obj->transforms = (struct matrixTransform*)realloc(obj->transforms, (obj->transformCount + 1) * sizeof(struct matrixTransform));
	}
	obj->transforms[obj->transformCount] = transform;
	obj->transformCount++;
}


