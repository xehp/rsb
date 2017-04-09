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


// Some of these numbers are also used in hello.js and/or on.js if changing here remember to change there also.
enum NumericEvent {
	NumericEventTick = 0,
	NumericEventKeyDown = 1,
	NumericEventKeyUp = 2,
	NumericEventMovementX = 3,
	NumericEventMovementY = 4,
	NumericEventKeyPress = 5
};


// Some of these numbers are also used in hello.js and/or on.js if changing here remember to change there also.
enum StringEvent {
	StringEventTick = 0,
	StringEventOnOpen = 1,
	StringEventOnError = 2,
	StringEventOnMessage = 3,
	StringEventOnClose = 4,
	StringEventMouseButtonDown = 5,
	StringEventMouseButtonUp = 6,
	StringEventMouseWheel = 7,
};



// These are the functions to be exported to JavaScript main, used from hybrid/hello.js.
// -s EXPORTED_FUNCTIONS="['_mainInit', '_mainHandleEvent', '_mainCleanup']"
extern "C" int mainHandleNumericEvent(const NumericEvent eventId, int eventData);
extern "C" const char * mainHandleStringEvent(const StringEvent eventId, const char* eventData);
//extern "C" int main(int argc, char** argv);


#endif /* HELLO_WORLD_SDL2_H_ */
