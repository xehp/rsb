/*
 * WindowObject.cpp
 *
 *  Created on: Feb 17, 2017
 *      Author: henrik
 */

#include "includeGlEtc.h"
#include "WindowObject.h"

WindowObject::WindowObject(int width, int height):
	MAIN_SCREEN_WIDTH(width),
	MAIN_SCREEN_HEIGHT(height),
	mainWindow(NULL),
	mainRenderer(NULL),
	mainGlContext(NULL)
{
}

WindowObject::~WindowObject() {
	assert(mainWindow==NULL);
	assert(mainGlContext==NULL);
}


int WindowObject::init()
{

	//SDL_CreateWindowAndRenderer(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN, &mainWindow, &mainRenderer);
	mainWindow = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (mainWindow == NULL)
	{
		printf( "SDL could not create window, SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}



	mainGlContext = SDL_GL_CreateContext(mainWindow);

	if (mainGlContext == NULL)
	{
		printf( "SDL could not create GL context, SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}


	//Initialize GLEW

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK )
	{
		printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
	}



	// Use Vsync
	if( SDL_GL_SetSwapInterval( 1 ) < 0 )
	{
		printf( "Set swap interval failed, SDL Error: %s\n", SDL_GetError() );
		return -1;
	}

	// SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, int index, Uint32 flags)
	mainRenderer = SDL_CreateRenderer(mainWindow, 0, SDL_RENDERER_ACCELERATED);
	if (mainRenderer == NULL)
	{
		printf( "SDL could not create renderer, SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}

	// Check that we have accelerated OpenGL and target texture.
	SDL_RendererInfo displayRendererInfo;
	SDL_GetRendererInfo(mainRenderer, &displayRendererInfo);
	if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0) {
		printf("renderer is not accelerated\n");
		return -1;
	}
	if ((displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
		printf("no render target texture\n");
		return -1;
	}

	// OK
	return 0;
}


int WindowObject::cleanup()
{

    SDL_DestroyRenderer(mainRenderer);
    mainRenderer=NULL;

	SDL_GL_DeleteContext(mainGlContext);
	mainGlContext=NULL;

	SDL_DestroyWindow(mainWindow);
	mainWindow=NULL;

	return 0;
}


// This command tells GL to show the newly rendered buffer (replacing the previous).
void WindowObject::renderIt()
{
	SDL_RenderPresent(mainRenderer);  // It is also possible to use glutSwapBuffers() here
}

/**
 * y is given in pixels while open GL uses floats
 * where (-1,-1) is lower left corner and (1,1) is the opposite corner.
 * So MAIN_SCREEN_HEIGHT corresponds to 2.0 in GL Float.
 */
float WindowObject::scale2dPixelToGlFloatY(int y)
{
	return ((float)y)/(MAIN_SCREEN_HEIGHT/2);
}

float WindowObject::scaleGlFloatTo2dPixelY(float yHeight)
{
	return (MAIN_SCREEN_HEIGHT*yHeight)/2.0f;
}

float WindowObject::scale2dPixelToGlFloatX(int x)
{
	return ((float)x)/(MAIN_SCREEN_WIDTH/2);
}

float WindowObject::scaleGlFloatTo2dPixelX(float xHeight)
{
	return (MAIN_SCREEN_WIDTH*xHeight)/2.0f;
}

float WindowObject::mainAspectRatio()
{
	return (MAIN_SCREEN_WIDTH+0.0f) / (MAIN_SCREEN_HEIGHT+0.0f);
}


float WindowObject::translateScreenToNormatedY(int y)
{
	float f = 1.0 - (((float)y / (float)MAIN_SCREEN_HEIGHT) * 2.0);
	return f;
}
