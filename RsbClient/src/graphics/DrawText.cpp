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

#include <assert.h>
#include "includeGlEtc.h"
#include "HybridLib.h"
#include "ImageLoader.h"
#include "DrawText.h"


// http://www.willusher.io/sdl2%20tutorials/2013/12/18/lesson-6-true-type-fonts-with-sdl_ttf/
// http://forums.tigsource.com/index.php?topic=15853.0;wap2

// TODO Need to Read more about text rendering, perhps this page:
// https://learnopengl.com/#!In-Practice/Text-Rendering



#include "ImageLoader.h"


DrawText *drawTextBig=NULL;;
DrawText *drawTextSmall=NULL;;


static int nextPowerOf2(int a)
{
	int i=1;
	while(i<a)
	{
		i*=2;
	}
	return i;
}



DrawText::DrawText(const char *fileName)
{
	const char* filename=fileName;


	printf("loading image file '%s'\n", filename);

	fontImage = new ImageLoader(); // delete of fontImage is done some some lines down in this method if load failed, or in drawTextQuit() if load was OK.
	assert(fontImage!=NULL);

	int r = fontImage->load_Image(filename);

	if (r>=0)
	{
		// OK


		xSize=fontImage->imageWidth/256;
		ySize=xSize*2;

		nRows=fontImage->imageHeight/ySize;
	}
	else
	{
		delete(fontImage);
		fontImage = NULL;
		xSize=0;
		ySize=0;
		nRows=0;
	}

}


int DrawText::init(const char *fontFileBig, const char *fontFileSmall)
{
	drawTextBig = new DrawText(fontFileBig);
	if ((drawTextBig==NULL) || (drawTextBig->nRows==0))
	{
		return -1;
	}

	drawTextSmall = new DrawText(fontFileSmall);
	if ((drawTextSmall==NULL) || (drawTextSmall->nRows==0))
	{
		return -1;
	}

	//drawText = new DrawText("courier8x16.png");
	return 0;
}

// c is the character position, where in the image the character shall be written.
// ch the type character to add (abc etc)
// dst the image buffer into which pixels are to be written
// isx image size X alias width, unit is number of pixels
void DrawText::addCharToImage(int c, unsigned int ch, unsigned char *dst, const int sx)
{
	//printf("addCharToImage '%c' %d\n", ch, ch);

	if (ch>=(nRows*256))
	{
		printf("%04x",ch);
		ch='?';
	}


	const int xs=xSize; // step for each character
	const int ys=ySize; // y alias image height
	const int row=ch&0xFF;
	const int col=ch>>8;
	const int bpp=fontImage->bytesPerPixel; // Bytes per pixel
	const int xoi=xSize*row; // X offset in courier image, based on character to draw
	const int yoi=ySize*col; // Y offset in courier image, based on character to draw
	const unsigned char *bitMapSrs=fontImage->imagePixels; // the bitmap containing characters that we can copy character bitmaps from.

	assert(col<nRows);

	// Rearranging the data order, swap rows/columns
	for(int x=0;x<xs;++x)
	{
		for(int y=0;y<ys;++y)
		{
			const int ii = c*xs + x + y*(sx);
			const int jj = (xoi + x) + (yoi+y)*(fontImage->imageWidth);

			const int i = ii * bpp;
			const int j = jj * bpp;
			dst[i]=bitMapSrs[j+0];
			dst[i+1]=bitMapSrs[j+1];
			dst[i+2]=bitMapSrs[j+2];
			dst[i+3]=bitMapSrs[j+3];
		}
	}
}




GLuint DrawText::textureFactoryW(const wchar_t *str, const int strLen, int charactersInBox)
{
	GLuint textureId=ZERO_GL;

	//printf("drawTextTextureFactory '%s' using %d\n", str, fontImage->imageWidth);

	const int xs=xSize; // step for each character
	const int ys=ySize; // y alias image height
	const int bpp=fontImage->bytesPerPixel; // Bytes per pixel

	// needed size in pixels X
	const int sx = xs*charactersInBox;

	// it must be a power of two
	int sx_p2=nextPowerOf2(sx);

	// Make it double in size if text is long.
	if ((strLen>charactersInBox) && (xSize>8))
	{
		// When displayed the image will be squeezed into same box so this makes the font smaller on screen.
		sx_p2*=2;
	}

	// In openGL ES textures must have size that is a power of 2.

	unsigned char *tmp=(unsigned char *)malloc(sx_p2*ys*bpp); // free is to be done later in this method
	assert(tmp!=NULL);

	memset(tmp,0,sx_p2*ys*bpp);

	for(int c=0;c<strLen;++c)
	{
		addCharToImage(c, str[c], tmp, sx_p2);
	}

	glGenTextures( 1, &textureId );
	glBindTexture( GL_TEXTURE_2D, textureId );

#if !defined(__EMSCRIPTEN__) && !(defined __APPLE__)
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
		gluBuild2DMipmaps( GL_TEXTURE_2D, fontImage->iFormat, sx_p2, ys, fontImage->gFormat, GL_UNSIGNED_BYTE, tmp);
#else
		// glTexParameteri tells how this texture is resized if needed.
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, fontImage->iFormat, isx, ys, 0, fontImage->gFormat, GL_UNSIGNED_BYTE, tmp);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sx_p2, ys, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
#endif

	free(tmp);

	return textureId;  // Remember to free texture when no longer needed by calling glDeleteTextures
}

// Returns the "name" of a generated texture (in OpenGl names are unsigned integers).
// Remember to free texture when no longer needed by calling glDeleteTextures
// str is a pointer to the string that shall be written on the texture
// charactersInBox is The size in characters that the box shall have.
// Returns ZERO_GL if not OK
// Deprecated, use drawTextTextureFactoryW
GLuint DrawText::textureFactory(const char *str, int charactersInBox)
{
	//printf("drawTextTextureFactory '%s' %d\n", str, charactersInBox);

	GLuint textureId=ZERO_GL;


	const int len=strlen(str);
	const int wlen = len*sizeof(wchar_t);
	wchar_t *wstr=(wchar_t *)malloc(wlen+sizeof(wchar_t));
	for(int i=0;i<len;++i)
	{
		//wstr[i]=(str[i]%64)+256*4;
		wstr[i]=str[i];
	}
	wstr[len]=0;

	textureId = textureFactoryW(wstr, len, charactersInBox);

	free(wstr);


	return textureId;  // Remember to free texture when no longer needed by calling glDeleteTextures
}


DrawText::~DrawText()
{
	if (fontImage!=NULL)
	{
		delete(fontImage);
		fontImage=NULL;
	}
}




// This shall be called before program exits to clean things up.
void DrawText::quit()
{
	if (drawTextBig!=NULL)
	{
		delete(drawTextBig);
		drawTextBig=NULL;
	}
	if (drawTextSmall!=NULL)
	{
		delete(drawTextSmall);
		drawTextSmall=NULL;
	}
}








