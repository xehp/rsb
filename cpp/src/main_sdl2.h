/*
 * hello_world_sdl2.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Mar 14, 2015
 *      Author: henrik
 */

#ifndef HELLO_WORLD_SDL2_H_
#define HELLO_WORLD_SDL2_H_

/*
#include <GL/glew.h>
#ifndef __EMSCRIPTEN__
#include <glm/glm.hpp>
#else
#include <glm.hpp>
#endif
*/



// These are the functions to be exported to JavaScript main, used from hybrid/hello.js.
// -s EXPORTED_FUNCTIONS="['_mainInit', '_mainHandleEvent', '_mainCleanup']"
extern "C" int mainHandleNumericEvent(int eventId, int eventData);
extern "C" const char * mainHandleStringEvent(int eventId, const char* eventData);
//extern "C" int main(int argc, char** argv);


#endif /* HELLO_WORLD_SDL2_H_ */
