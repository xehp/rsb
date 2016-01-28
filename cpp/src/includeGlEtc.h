// standard IO, and, strings
#include <stdio.h>
#include <string>
#include <iostream>

#include <GL/glew.h>


#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
//#include <SDL2/SDL.h>
#include <SDL.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


//Using SDL, SDL OpenGL
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>


#ifndef __EMSCRIPTEN__
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#else
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#endif
using namespace glm;

// #include <SOIL/SOIL.h> // http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06

#ifndef __EMSCRIPTEN__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#endif


#ifdef __EMSCRIPTEN__
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <emscripten.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
//#include <png.h>
//#include <zlib.h>
#endif

#include <GL/glu.h>




#include "CreateProgram.h"



// Do not change this, it shall be 0, http://stackoverflow.com/questions/4600412/what-is-opengls-method-for-assigning-texture-ids
#define ZERO_GL 0


