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



class ImageLoader;
class DrawText;

extern DrawText *drawTextBig;
extern DrawText *drawTextSmall;
class DrawText
{
public:


	DrawText(const char *fileName);
	~DrawText();

	//void drawTextRender(int x, int y, SDL_Window* displayWindow, SDL_Renderer* displayRenderer);
	static int init(const char *fontFileBig, const char *fontFileSmall);
	static void quit();
	GLuint textureFactory(const char *str, int charactersInBox);
	GLuint textureFactoryW(const wchar_t *str, const int strLen, int charactersInBox);
	void addCharToImage(int c, unsigned int ch, unsigned char *dst, const int sx);

	int getSizeX() const {return xSize;};
	int getSizeY() const {return ySize;};

	ImageLoader *fontImage;


	int xSize;
	int ySize;
	int nRows;
};

#endif
