//
//  includes.h
//  C-Ray
//
//  Created by Valtteri Koskivuori on 28/02/15.
//  Copyright (c) 2015 Valtteri Koskivuori. All rights reserved.
//

#pragma once

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

//Global constants
#define MAX_CRAY_VERTEX_COUNT 3
#define PIOVER180 0.017453292519943295769236907684886
#define PI        3.141592653589793238462643383279502

//Some macros
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define invsqrt(x) (1.0 / sqrt(x))

//Master include file
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <signal.h>
#include <string.h>

//These are for multi-platform physical core detection
//#ifdef MACOS
//#include <sys/param.h>
//#include <sys/sysctl.h>
//#elif _WIN32
//#include <windows.h>
//#else
#include <unistd.h>
//#endif

//Project includes
#define MATERIAL_NAME_SIZE 255
#define OBJ_FILENAME_LENGTH 500

#include "color.h"
#include "material.h"
#include "vector.h"
#include "list.h"
#include "sphere.h"
#include "transforms.h"
#include "lightRay.h"

enum fileMode {
	loadModeNormal,//Load scene def normally
	loadModeTarga, //Load previous render state
	saveModeNormal,//Save image
	saveModeTarga, //Save current render state
	saveModeNone   //Don't save
};