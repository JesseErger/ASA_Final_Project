//
//  filehandler.c
//  C-Ray
//
//  Created by Valtteri Koskivuori on 28/02/15.
//  Copyright (c) 2015 Valtteri Koskivuori. All rights reserved.
//

#include "includes.h"
#include "filehandler.h"

#include "camera.h"
#include "scene.h"
#include "renderer.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

//Prototypes for internal functions
extern struct renderer mainRenderer;
int getFileSize(char *fileName);

void saveBmpFromArray(const char *filename, unsigned char *imgData, int width, int height) {
	
	//Apparently BMP is BGR, whereas C-Ray's internal buffer is RGB (Like it should be)
	//So we need to convert the image data before writing to file.
	
	unsigned char *bgrData = (unsigned char*)calloc(3 * width * height, sizeof(unsigned char));
	
	//FIXME: For some reason we can't access the 0 of X and Y on imgdata. So now BMP images have 1 black row on left and top edges...
	for (int y = 1; y < height; y++) {
		for (int x = 1; x < width; x++) {
			bgrData[(x + (height - y) * width) * 3 + 0] = imgData[(x + (height - y) * width) * 3 + 2];
			bgrData[(x + (height - y) * width) * 3 + 1] = imgData[(x + (height - y) * width) * 3 + 1];
			bgrData[(x + (height - y) * width) * 3 + 2] = imgData[(x + (height - y) * width) * 3 + 0];
		}
	}
	
	int i;
	int error;
	FILE *f;
	int filesize = 54 + 3 * width * height;
	
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppadding[3] = {0,0,0};
	
	//Create header with filesize data
	bmpfileheader[2] = (unsigned char)(filesize    );
	bmpfileheader[3] = (unsigned char)(filesize>> 8);
	bmpfileheader[4] = (unsigned char)(filesize>>16);
	bmpfileheader[5] = (unsigned char)(filesize>>24);
	
	//create header width and height info
	bmpinfoheader[ 4] = (unsigned char)(width    );
	bmpinfoheader[ 5] = (unsigned char)(width>>8 );
	bmpinfoheader[ 6] = (unsigned char)(width>>16);
	bmpinfoheader[ 7] = (unsigned char)(width>>24);
	
	bmpinfoheader[ 8] = (unsigned char)(height    );
	bmpinfoheader[ 9] = (unsigned char)(height>>8 );
	bmpinfoheader[10] = (unsigned char)(height>>16);
	bmpinfoheader[11] = (unsigned char)(height>>24);
	
	f = fopen(filename,"wb");
	error = (unsigned int)fwrite(bmpfileheader,1,14,f);
	if (error != 14) {
		printf("Error writing BMP file header data\n");
	}
	error = (unsigned int)fwrite(bmpinfoheader,1,40,f);
	if (error != 40) {
		printf("Error writing BMP info header data\n");
	}
	
	for (i = 1; i <= height; i++) {
		error = (unsigned int)fwrite(bgrData+(width*(height - i)*3),3,width,f);
		if (error != width) {
			printf("Error writing image line to BMP\n");
		}
		error = (unsigned int)fwrite(bmppadding,1,(4-(width*3)%4)%4,f);
		if (error != (4-(width*3)%4)%4) {
			printf("Error writing BMP padding data\n");
		}
	}
	fclose(f);
}

int getFileSize(char *fileName) {
	FILE *file;
	file = fopen(fileName, "r");
	if (!file) return 0;
	fseek(file, 0L, SEEK_END);
	int size = (int)ftell(file);
	fclose(file);
	return size;
}

int writeTarga(struct renderer *renderer, char *fileName) {
	//We can just write the renderer to memory
	FILE *file;
	file = fopen(fileName, "wb");
	fwrite(&renderer, sizeof(renderer), 1, file);
	fclose(file);
	return 0;
}

struct renderer *readTarga(char *fileName) {
	FILE *file;
	file = fopen(fileName, "r");
	struct renderer *renderer = (struct renderer*)malloc(sizeof(struct renderer));
	fread(&renderer, sizeof(renderer), 1, file);
	return renderer;
}
