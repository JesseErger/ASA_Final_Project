//
//  ui.h
//  C-Ray
//
//  Created by Valtteri Koskivuori on 19/02/2017.
//  Copyright © 2017 Valtteri Koskivuori. All rights reserved.
//

#pragma once

#ifdef UI_ENABLED
struct display {
	bool isBorderless;
	bool isFullScreen;
};

int initSDL();
void drawWindow();
extern struct display mainDisplay;
#endif

void printDuration(double time);
void getKeyboardInput();
