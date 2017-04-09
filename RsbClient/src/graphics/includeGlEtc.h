// standard IO, and, strings
#include <stdio.h>
#include <string>
#include <iostream>

#ifndef RSB_SERVER


//Using SDL, SDL OpenGL
#include <GL/glew.h>
#ifdef __APPLE__
//#include <GLUT/glu.h>
#else
#include <GL/glu.h>
#endif

// http://stackoverflow.com/questions/23031740/how-to-use-opengl-with-gcc-on-mac
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// TODO can we use GLFW instead of GLUT/FreeGLUT
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>



#ifndef __EMSCRIPTEN__
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#else
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#endif


using namespace glm;

#include <SDL_image.h>
//#include <SDL_ttf.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

//#include <GL/glu.h>




#include "CreateProgram.h"



// Do not change this, it shall be 0, http://stackoverflow.com/questions/4600412/what-is-opengls-method-for-assigning-texture-ids
#define ZERO_GL 0


#endif // RSB_SERVER

