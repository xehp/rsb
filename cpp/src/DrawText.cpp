/*
DrawText.cpp


www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


/*

The purpose of this code is to create a texture with text in it.





Simple Example for compiling to an object file:

gcc -c `sdl-config --cflags` mysource.c

Simple Example for linking an executable (Unix style has no .exe):

gcc -o myprogram mysource.o `sdl-config --libs` -lSDL_ttf

*/

// Using USE_SDL_TTF does not work in web build. Perhaps this gives a hint on how to fix it:
// https://groups.google.com/forum/#!topic/emscripten-discuss/lfXaVdyYztw


#include "includeGlEtc.h"
#include "HybridLib.h"
#include "ImageLoader.h"
#include "DrawText.h"


// http://www.willusher.io/sdl2%20tutorials/2013/12/18/lesson-6-true-type-fonts-with-sdl_ttf/
// http://forums.tigsource.com/index.php?topic=15853.0;wap2



#ifdef TRY_TO_USE_STB
#include <fstream>
#include <iterator>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


//#include "stb_truetype.h"

   stbtt_fontinfo stbttFontinfo;


#endif

#ifdef USE_IMAGE_LOADER_FONT
#include "ImageLoader.h"
#endif

#ifdef USE_SDL_TTF
   TTF_Font *ttfFont=NULL;
#endif

#if defined(TRY_TO_USE_PNG_IMG)
	SDL_Surface *courierSurface=NULL;
	int courierSurfaceInternalFormat=0;
	int charHeight=0;
	int charWidth=0;
#endif


#if defined(TRY_TO_USE_PNG_IMG)
	void draw_text_char_cordinates(int ch, SDL_Rect* srcrect)
	{
		srcrect->x=((1791*ch)/128); // ~14
		srcrect->y=0;
		srcrect->w=14;
		srcrect->h=32;

	}

#endif

#ifdef USE_IMAGE_LOADER_FONT
#ifndef USE_ALT_IMG
#error // need USE_ALT_IMG
#endif
	ImageLoader *drawTextFontImage=NULL;
#endif

static int nextPowerOf2(int a)
{
	int i=1;
	while(i<a)
	{
		i*=2;
	}
	return i;
}

#if defined(TRY_TO_USE_PNG_IMG)
static GLuint generateTextureFromSurface(SDL_Surface *tmpSurface)
{
	GLuint textureId=ZERO_GL;
	int tmpInternalFormat,tmpFormat;

	// try guess what format to tell glTexImage2D to use.
	if (tmpSurface->format->BytesPerPixel == 4)
	{
		tmpInternalFormat = GL_RGBA8; // RGBA 32bit
		tmpFormat = GL_RGBA; // RGBA 32bit
	}
	else
	{
			printf("unknown image format %d\n", tmpSurface->format->BytesPerPixel);
			return ZERO_GL;
	}


	// create one texture name
	glGenTextures(1, &textureId);

	// tell opengl to use the generated texture name
	glBindTexture(GL_TEXTURE_2D, textureId);

	// this reads from the sdl surface and puts it into an opengl texture
	glTexImage2D(GL_TEXTURE_2D, 0, tmpInternalFormat, tmpSurface->w, tmpSurface->h, 0, tmpFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	return textureId;
}

#endif

// Returns the "name" of a generated texture (in OpenGl names are unsigned integers).
// Remember to free texture when no longer needed by calling glDeleteTextures
// Returns ZERO_GL if not OK
GLuint drawTextTextureFactory(const char *str, int charactersInBox)
{
	GLuint textureId=ZERO_GL;

#ifdef USE_SDL_TTF

	if (ttfFont==NULL)
	{
		return ZERO_GL;
	}
	else
	{
		int internal_format=0;
		int texture_format=0;


		const SDL_Color WhiteColor = {255, 255, 255, 0xff};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

		//SDL_Surface * sdlSurface = TTF_RenderText_Solid(ttfFont, str, WhiteColor);
		SDL_Surface * tmpSurface = TTF_RenderUTF8_Solid(ttfFont, str, WhiteColor); // Can not get solid to work. Not this or  TTF_RenderText_Solid.
		//SDL_Surface * tmpSurface = TTF_RenderUTF8_Blended(ttfFont, str, WhiteColor); // This works on native but not web target. It is slow so I do wan't solid anyway.  With emscripten this result in: warning: unresolved symbol: TTF_RenderUTF8_Blended

		if( tmpSurface == NULL )
		{
			printf("Failed to render text '%s', SDL_Error: %s\n",str, SDL_GetError());
			return ZERO_GL;
		}

		printf("tmpSurface %d %d   %d 0x%08x\n", tmpSurface->w, tmpSurface->h, tmpSurface->format->BytesPerPixel, tmpSurface->format->Rmask);

		// OpenGL ES requires size to be a power of two, so here we copy into a larger image.
		SDL_Surface *sdlSurface = SDL_CreateRGBSurface(0, nextPowerOf2(tmpSurface->w), nextPowerOf2(tmpSurface->h), 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	    SDL_BlitSurface(tmpSurface, NULL, sdlSurface, NULL);



		// http://stackoverflow.com/questions/5289447/using-sdl-ttf-with-opengl
		const Uint8 bytesPerPixel = sdlSurface->format->BytesPerPixel;
		if (bytesPerPixel == 4) {   // alpha
	    	internal_format = GL_RGBA8;
		    if (sdlSurface->format->Rmask == 0x000000ff)
		        texture_format = GL_RGBA;
		    else
		        texture_format = GL_BGRA;
		}
		else if (bytesPerPixel == 3) {             // no alpha
	    	internal_format = GL_RGB8;
		    if (sdlSurface->format->Rmask == 0x000000ff)
		        texture_format = GL_RGB;
		    else
		        texture_format = GL_BGR;
		}
		else
		{
			printf("failed to guess internal format %d\n", bytesPerPixel);
		}


		printf("sdlSurface %d %d   %d 0x%08x %d  %d\n", sdlSurface->w, sdlSurface->h, bytesPerPixel, sdlSurface->format->Rmask, texture_format, GL_RGBA);




		// create one texture name
		glGenTextures(1, &textureId);

		// tell opengl to use the generated texture name
		glBindTexture(GL_TEXTURE_2D, textureId);

		// this reads from the sdl surface and puts it into an opengl texture
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, sdlSurface->w, sdlSurface->h, 0, texture_format, GL_UNSIGNED_BYTE, sdlSurface->pixels);

		// these affect how this texture is drawn later on...
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);


		printf("drawTextTextureFactory %d %d\n", sdlSurface->w, sdlSurface->h);

		// clean up
		SDL_FreeSurface(tmpSurface);
		SDL_FreeSurface(sdlSurface);

	}
#elif defined(TRY_TO_USE_STB)

	// Not implemented yet

#elif defined(TRY_TO_USE_PNG_IMG)

	printf("drawTextTextureFactory: %s\n", str);




	const int strLen=strlen(str);

	// https://wiki.libsdl.org/SDL_CreateRGBSurface
	// Create a surface, TODO: Byte Order mask? How do we handle it?
	SDL_Surface * tmpSurface = SDL_CreateRGBSurface(0,nextPowerOf2(16*(strLen<16?16:strLen)),nextPowerOf2(32),32,0x000000ff,0x0000ff00,0x00ff0000,0xff000000); // this works on native but not 100 % on web
	//SDL_Surface * tmpSurface = SDL_CreateRGBSurface(0,256,32,32,0x000000ff,0x0000ff00,0x00ff0000,0xff000000); // this works on native but not 100 % on web
	//SDL_Surface * tmpSurface = SDL_CreateRGBSurface(0,256,32,32,0,0,0,0); // this does not work.

	// blit characters into it.


	//SDL_Color GreyColor = {0x40, 0x40, 0x40, 0xff};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	SDL_FillRect(tmpSurface, NULL, 0x40404080);



	SDL_Rect srcrect;
	SDL_Rect dstrect;


	int x=0;
	for(int i=0; i<strLen; ++i)
	{

		draw_text_char_cordinates(str[i], &srcrect);

		dstrect.x=x;
		dstrect.y=0;
		dstrect.w=srcrect.w;
		dstrect.h=srcrect.h;

		x+=srcrect.w;

		SDL_FillRect(tmpSurface, &dstrect, 0x80004080);

		// https://wiki.libsdl.org/SDL_BlitSurface
		const int r = SDL_BlitSurface(courierSurface,
										&srcrect,
										tmpSurface,
										&dstrect);
		if (r!=0)
		{
			printf("SDL_BlitSurface failed %s\n", SDL_GetError());
			return ZERO_GL;
		}
	}

	// Now generate texture

	textureId = generateTextureFromSurface(tmpSurface);



	// these affect how this texture is drawn later on...
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Free resources no longer needed
	SDL_FreeSurface(tmpSurface);
	//SDL_FreeSurface(copySurface);


#elif defined(USE_IMAGE_LOADER_FONT)

	//printf("drawTextTextureFactory '%s' using %d\n", str, drawTextFontImage->imageWidth);

	const int strLen=strlen(str);


	unsigned char *ptr=drawTextFontImage->imagePixels;
	const int xs=16; // step for each character
	const int ys=32; // y alias image height
	const int bpp=drawTextFontImage->bytesPerPixel; // Bytes per pixel

	const int sx = strLen<charactersInBox?xs*charactersInBox:xs*strLen;

	// In openGL ES textures must have size that is a power of 2.
	const int isx=nextPowerOf2(sx); // image size X alias width

	unsigned char *tmp=(unsigned char *)malloc(isx*ys*bpp); // free is to be done later in this method
	assert(tmp!=NULL);

	memset(tmp,0,isx*ys*bpp);

	for(int c=0;c<strLen;++c)
	{
		const int ch = str[c] /* 48+c*/;

		//const int xoi=((1791*ch)/128); // ~14   // X offset in courier image
		const int xoi=16*ch; // X offset in courier image

		for(int x=0;x<xs;++x)
		{
			for(int y=0;y<ys;++y)
			{
				const int ii = c*xs + x + y*(isx);
				const int jj = xoi + x + y*(drawTextFontImage->imageWidth);

				const int i = ii * bpp;
				const int j = jj * bpp;
				tmp[i]=ptr[j+0];
				tmp[i+1]=ptr[j+1];
				tmp[i+2]=ptr[j+2];
				tmp[i+3]=ptr[j+3];
			}
		}
	}

	glGenTextures( 1, &textureId );
	glBindTexture( GL_TEXTURE_2D, textureId );


#ifndef __EMSCRIPTEN__
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
		gluBuild2DMipmaps( GL_TEXTURE_2D, drawTextFontImage->iFormat, isx, ys, drawTextFontImage->gFormat, GL_UNSIGNED_BYTE, tmp);
#else
		// glTexParameteri tells how this texture is resized if needed.
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, d->iFormat, d->imageWidth, d->imageHeight, 0, d->gFormat, GL_UNSIGNED_BYTE, d->imagePixels);
		glTexImage2D(GL_TEXTURE_2D, 0, drawTextFontImage->iFormat, isx, ys, 0, drawTextFontImage->gFormat, GL_UNSIGNED_BYTE, tmp);
#endif

	free(tmp);

#else
#error
#endif
	return textureId;  // Remember to free texture when no longer needed by calling glDeleteTextures
}


// This shall be called before drawTextTextureFactory is used
int drawTextInit()
{
	#ifdef USE_SDL_TTF
		//if(!TTF_WasInit())
		{
			if(TTF_Init()==-1) {
				printf("TTF_Init: %s\n", TTF_GetError());
				return -1;
			}
			else
			{

				const char *fontName="FreeSerif.ttf"; // Or try: FreeSerif.ttf GFX/exprswy free.ttf    /usr/share/fonts/truetype/freefont/FreeSerif.ttf  /home/henrik/git/emscripten-sdl2-gl/RsbClient/Debug/FreeSerif.ttf /usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf  arial.ttf

				ttfFont = TTF_OpenFont(fontName , 20 );
				if (ttfFont==NULL)
				{
					std::cerr << "Error loading font: '" << fontName << "', TTF_GetError: '" << TTF_GetError() << "'" << std::endl;
					return -1;
				}
				else
				{
					printf("drawTextInit: %s OK\n",fontName);
				}
			}
		}
	#elif defined(TRY_TO_USE_STB)
		const char *fontName="FreeSerif.ttf";

		// http://stackoverflow.com/questions/18816126/c-read-the-whole-file-in-buffer
		std::ifstream file(fontName, std::ios::binary);
		file.seekg(0, std::ios::end);
		std::streamsize fsize = file.tellg();
		file.seekg(0, std::ios::beg);

		if (fsize>0)
		{

			std::vector<char> buffer(fsize);

			printf("file '%s', size %d\n", fontName, (int)fsize);

			file.read(buffer.data(), fsize);

			const std::streamsize gcount = file.gcount();

			file.close();

			if (gcount==fsize)
			{
				unsigned char* udata=(unsigned char*)buffer.data();

				stbtt_InitFont(&stbttFontinfo, udata, 0);
			}
			else
			{
				return ZERO_GL;
			}
		}
		else
		{
			printf("font file not found or is empty '%s'\n", fontName);
			return ZERO_GL;
		}

	#elif defined(TRY_TO_USE_PNG_IMG)

		//GLuint textureId;
		const char* filename="courier.png";



		printf("loading image file '%s'\n", filename);

		courierSurface = IMG_Load(filename);


		// could not load filename
		if (!courierSurface) {
				printf("could not load image %s\n", filename);
				return ZERO_GL;

		}

		// try guess what format to tell glTexImage2D to use.
		if (courierSurface->format->BytesPerPixel == 4)
		{ // RGBA 32bit
			courierSurfaceInternalFormat = GL_RGBA;
		}
		else
		{
				printf("unknown image format %d\n", courierSurface->format->BytesPerPixel);
				return ZERO_GL;
		}


		printf("courierSurface %d %d\n", (int)courierSurface->w, (int)courierSurface->h);

		charWidth = courierSurface->w/16;
		charHeight = courierSurface->h/8;



	#elif defined(USE_IMAGE_LOADER_FONT)

		#if defined(USE_IMG_LOAD) || defined(USE_ALT_IMG_STB)
				const char* filename="courier.png";
		#elif defined(USE_ALT_IMG)
				const char* filename="courier.bmp";
		#else
		#error
		#endif


		printf("loading image file '%s'\n", filename);

		assert(drawTextFontImage==NULL);
		drawTextFontImage = new ImageLoader(); // delete of drawTextFontImage is done some some lines down in this method if load failed, or in drawTextQuit() if load was OK.
		assert(drawTextFontImage!=NULL);

		int r = drawTextFontImage->load_Image(filename);

		if (r>=0)
		{
			// OK
		}
		else
		{
			delete(drawTextFontImage);
			drawTextFontImage = NULL;
		}

	#endif
	return 0;
}


// This shall be called before program exits to clean things up.
void drawTextQuit()
{
	#ifdef USE_SDL_TTF
	TTF_CloseFont(ttfFont);
	#endif


	#ifdef USE_IMAGE_LOADER_FONT
	if (drawTextFontImage!=NULL)
	{
		delete(drawTextFontImage);
		drawTextFontImage=NULL;
	}
	#endif

	#if defined(TRY_TO_USE_PNG_IMG)
		if (courierSurface)
		{
			SDL_FreeSurface(courierSurface);
			courierSurface=NULL;
		}
	#endif


	#ifdef USE_SDL_TTF
		TTF_Quit();
	#endif
}









