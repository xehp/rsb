/*
 * MirrorContainer.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *
 *  Created on: Feb 12, 2017
 *      Author: henrik
 */

#ifndef SRC_MirrorContainer_H_
#define SRC_MirrorContainer_H_

#include "SDL_video.h" // TODO can we forward declare SDL_GLContext some other way? Its just a void* anyway.
#include "MirrorContainer.h"

struct SDL_Window;
struct SDL_Renderer;


class WindowObject: public MirrorContainer
{
public:

	WindowObject(int width, int height);
	virtual ~WindowObject();
	virtual WindowObject* getWindowObject() {return this;}
	int init();
	int cleanup();
	float scale2dPixelToGlFloatY(int y);
	float scaleGlFloatTo2dPixelY(float yHeight);
	float scale2dPixelToGlFloatX(int x);
	float scaleGlFloatTo2dPixelX(float xHeight);
	float mainAspectRatio();
	float translateScreenToNormatedY(int y);
	void renderIt();


	int MAIN_SCREEN_WIDTH;
	int MAIN_SCREEN_HEIGHT;

	SDL_Window* mainWindow;
	SDL_Renderer* mainRenderer;
	SDL_GLContext mainGlContext;

};


#endif /* INPUTBUFFER_H_ */
