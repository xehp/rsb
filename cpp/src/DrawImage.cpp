/*
DrawImage.cpp


www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


/*

The purpose of this code is to create a texture with an image in it.

*/


#include "includeGlEtc.h"
#include "HybridLib.h"
#include "ImageLoader.h"
#include "DrawImage.h"









ImageLoader *drawImageLoader=NULL;


static int nextPowerOf2(int a)
{
	int i=1;
	while(i<a)
	{
		i*=2;
	}
	return i;
}


// Returns the "name" of a generated texture (in OpenGl names are unsigned integers).
// Remember to free texture when no longer needed by calling glDeleteTextures
// Returns ZERO_GL if not OK
GLuint drawImageTextureFactory(int xOffset, int yOffset, int width, int height)
{
	GLuint textureId=ZERO_GL;


	printf("drawImageTextureFactory %d %d %d %d\n", xOffset, yOffset, width, height);


	unsigned char *ptr=drawImageLoader->imagePixels;
	const int bpp=drawImageLoader->bytesPerPixel;


	// In openGL ES textures must have size that is a power of 2.
	height=nextPowerOf2(height);
	width=nextPowerOf2(width);

	size_t s = width*height*bpp;

	unsigned char *tmp=(unsigned char *)malloc(s); // free is to be done later in this method
	assert(tmp!=NULL);

	memset(tmp,0,s);

	for(int x=0;x<width;++x)
	{
		for(int y=0;y<height;++y)
		{
			const int ii = x + y*(width);
			const int jj = xOffset + x + y*(drawImageLoader->imageWidth);
			const int i = ii * bpp;
			const int j = jj * bpp;
			tmp[i]=ptr[j+0];
			tmp[i+1]=ptr[j+1];
			tmp[i+2]=ptr[j+2];
			tmp[i+3]=ptr[j+3];
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
		gluBuild2DMipmaps( GL_TEXTURE_2D, drawImageLoader->iFormat, width, height, drawImageLoader->gFormat, GL_UNSIGNED_BYTE, tmp);
#else
		// these affect how this texture is drawn later on...
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, d->iFormat, d->imageWidth, d->imageHeight, 0, d->gFormat, GL_UNSIGNED_BYTE, d->imagePixels);
		glTexImage2D(GL_TEXTURE_2D, 0, drawImageLoader->iFormat, width, height, 0, drawImageLoader->gFormat, GL_UNSIGNED_BYTE, tmp);
#endif

	free(tmp);

	return textureId;  // Remember to free texture when no longer needed by calling glDeleteTextures
}


// This shall be called before drawTextTextureFactory is used
int drawImageInit()
{
	const char* filename="snowscreen.png";


	printf("loading image file '%s'\n", filename);

	assert(drawImageLoader==NULL);
	drawImageLoader = new ImageLoader(); // delete of drawTextFontImage is done some some lines down in this method if load failed, or in drawTextQuit() if load was OK.
	assert(drawImageLoader!=NULL);

	int r = drawImageLoader->load_Image(filename);

	if (r>=0)
	{
		// OK
	}
	else
	{
		printf("failed to load image '%s'\n", filename);

		drawImageQuit();
	}

	return 0;
}


// This shall be called before program exits to clean things up.
void drawImageQuit()
{
	if (drawImageLoader!=NULL)
	{
		delete(drawImageLoader);
		drawImageLoader=NULL;
	}
}









