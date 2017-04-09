/*
 * HybridLib.cpp
 *
 *  Created on: Apr 30, 2015
 *      Author: henrik
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 */

#include <stdio.h>
#ifndef RSB_SERVER
#include "includeGlEtc.h"
#ifndef __EMSCRIPTEN__
#include "hca.h"
#endif
#include "HybridLib.h"

// TODO These variables dont need to be in this file.
int ourAvatarId=-1; // we need to know which object is our avatar so we can avoid rendering it (if we did we would only see the avatar from inside)

int mainLog = 0;


#ifndef __EMSCRIPTEN__
HcaSocket hcaSocket; // This replaces WebSockest in the native build.
#endif


// debugName shall be a string identifying the code that sends this debug message, usually the function name, __FILE__ can be used if nothing else is available.
void mainDebug(const char *debugName, const char* str)
{
	if (mainLog<100)
	{
		printf("mainDebug(%d),%.40s): %.160s\n",mainLog, debugName, str);
		++mainLog;
	}
}


// returns zero if OK
int mainSend(const char *str)
{
	mainDebug("mainSend",str);

	#ifdef __EMSCRIPTEN__
		// TODO: Can string (= char* buffers) be transferred to/from C code calling JS using the EM_ASM construct?
		// https://groups.google.com/forum/#!topic/emscripten-discuss/_M75jXb2-HY
		// Remember to not use double quotes inside EM_ASM!
		const int r = EM_ASM_INT({
			var s = Pointer_stringify($0);
			//Module.print('mainSend: ' + s);
			websocket.send(s);
			return 0;
		}, str);

		return r;
	#else
		const int r = hcaSocket.send(str);
		return r;
	#endif
}


// TODO Test if SDL_GetRelativeMouseMode works ok on web, remove this flag if so.
// The problem is we don't use SDL for this in web version, so for web version we need something else than SDL_GetRelativeMouseMode.
// Unfortunately this flag is not enough either since if user press "esc" key this flag is not changed.
//bool mouseIsCaptured=false;


bool mouseIsPointerLock()
{
	#ifndef __EMSCRIPTEN__
		return SDL_GetRelativeMouseMode();
	#else
		const int r = EM_ASM_INT( {return mainIsPointerLock();},0 );
		return r;
	#endif

}


void mouseCapture()
{
	printf("mouseCapture\n");
	#ifndef __EMSCRIPTEN__
		SDL_SetRelativeMouseMode(SDL_TRUE);
	#else
		EM_ASM( mainSetUpPointerLock() );
	#endif
	//mouseIsCaptured=true;
}

void mouseRelease()
{
	printf("mouseRelease\n");
	#ifndef __EMSCRIPTEN__
		SDL_SetRelativeMouseMode(SDL_FALSE);

		//int x,y;
		//SDL_GetRelativeMouseState(&x, &y);

	#else
		EM_ASM( mainReleasePointerLock() );
	#endif
	//mouseIsCaptured=false;
}


#endif

