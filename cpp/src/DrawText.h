/*
DrawText.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef DRAW_TEXT_H
#define DRAW_TEXT_H

#ifndef ZERO_GL
#define ZERO_GL 0
#endif

// USE_SDL_TTF:
// This works in native build.
// Using USE_SDL_TTF does not work in web build. Perhaps this gives a hint on how to fix it:
// https://groups.google.com/forum/#!topic/emscripten-discuss/lfXaVdyYztw


// TRY_TO_USE_STB:
// An alternative to SDL_TTF is "stb_truetype.h"
// Not implemented yet

// TRY_TO_USE_PNG_IMG:
// This too works on native but not always (sometimes it works but mostly not) on web :(
// Will try to cut and paste from a PNG image with characters.

// USE_IMAGE_LOADER_FONT:
// This loads the font as BMP or PNG file, it works on both native and target.


// Uncomment only one of the below alternatives
//#define USE_SDL_TTF
//#define TRY_TO_USE_STB
//#define TRY_TO_USE_PNG_IMG
#define USE_IMAGE_LOADER_FONT


//void drawTextRender(int x, int y, SDL_Window* displayWindow, SDL_Renderer* displayRenderer);
int drawTextInit();
void drawTextQuit();
GLuint drawTextTextureFactory(const char *str, int charactersInBox);

#endif
