/*
ImageLoader.cpp


www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include "includeGlEtc.h"
#include "HybridLib.h"

#include <vector>
#include <fstream>
#include <iostream>

// http://stackoverflow.com/questions/20308662/linking-sdl-image-in-make-file

#include <iostream>

#if defined(USE_ALT_IMG)
#include <fstream>
#include <iterator>
#include <vector>
#endif



//#include <GL/glu.h>

#ifdef USE_IMG_LOAD
#include <png.h>
#include <zlib.h>
#endif


#include "ImageLoader.h"


#ifdef USE_ALT_IMG_STB
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"
#endif




int imageLoaderInit()
{
#ifdef USE_IMG_LOAD

	//IMG_Init(IMG_INIT_PNG| IMG_INIT_JPG | IMG_INIT_TIF | IMG_INIT_WEBP);

	// Initialize PNG image
	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) )
	{
		printf( "IMG_Init failed: %s\n", IMG_GetError() );
		return -1;
	}
#elif defined(USE_ALT_IMG)
	// nothing
#else
#error
#endif


	return 0;
}

void imageLoaderQuit()
{
#ifdef USE_IMG_LOAD
	IMG_Quit();
#elif defined(USE_ALT_IMG)
	// nothing
#else
#error
#endif
}


#ifdef USE_IMG_LOAD

static GLuint demoImageTextureFromSurface(SDL_Surface *surface)
{
	GLuint textureId=ZERO_GL;
	int iFormat;
	int eFormat;

	// try guess what format to tell glTexImage2D to use.
	if (surface->format->BytesPerPixel == 3) { // RGB 24bit

			iFormat = GL_RGB8;
			eFormat = GL_RGB;

	} else if (surface->format->BytesPerPixel == 4) { // RGBA 32bit

		iFormat = GL_RGBA8;
		eFormat = GL_RGBA;

	} else {

			printf("unknown image format %d\n", surface->format->BytesPerPixel);

			SDL_FreeSurface(surface);
			return ZERO_GL;

	}


	// create one texture name
	glGenTextures(1, &textureId);

	// tell opengl to use the generated texture name
	glBindTexture(GL_TEXTURE_2D, textureId);

	// this reads from the sdl surface and puts it into an opengl texture
	glTexImage2D(GL_TEXTURE_2D, 0, iFormat, surface->w, surface->h, 0, eFormat, GL_UNSIGNED_BYTE, surface->pixels);

	return textureId;
}


#elif defined(USE_ALT_IMG)

static GLuint demoImageTextureFromImage(ImageLoader *demoImage)
{
	GLuint textureId=ZERO_GL;

	// http://stackoverflow.com/questions/12518111/how-to-load-a-bmp-on-glut-to-use-it-as-a-texture

	glGenTextures( 1, &textureId );
	glBindTexture( GL_TEXTURE_2D, textureId );

#if !defined(__EMSCRIPTEN__) && !(defined __APPLE__)
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
	gluBuild2DMipmaps( GL_TEXTURE_2D, demoImage->iFormat, demoImage->imageWidth, demoImage->imageHeight, demoImage->gFormat, GL_UNSIGNED_BYTE, demoImage->imagePixels);
#else
	// these affect how this texture is drawn later on...
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, demoImage->iFormat, demoImage->imageWidth, demoImage->imageHeight, 0, demoImage->gFormat, GL_UNSIGNED_BYTE, demoImage->imagePixels);
#endif


	return textureId;

}
#else
#error
#endif



// An id (AKA name) of the texture is returned.
// Caller must do glDeleteTextures when it is no longer needed.
// Returns ZERO_GL if it failed.
GLuint ImageLoaderLoadTexture(const char *filename, int *textureWidth, int *textureHeight)
{
	GLuint textureId=ZERO_GL;

	#ifdef USE_IMG_LOAD

	// http://sdl.beuc.net/sdl.wiki/OpenGL_Texture_Example
	SDL_Surface *surface;

	surface = IMG_Load(filename);


	// Or if you don't use SDL_image you can use SDL_LoadBMP here instead:
	// surface = SDL_LoadBMP(filename);

	// could not load filename
	if (!surface) {
			printf("could not load image %s\n", filename);
			return ZERO_GL;

	}

	if ((textureWidth!=NULL) && (textureHeight!=NULL))
	{
	 *textureWidth=surface->w;
	 *textureHeight=surface->h;
	}

	textureId = demoImageTextureFromSurface(surface);

	// these affect how this texture is drawn later on...
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// clean up
	SDL_FreeSurface(surface);
	#elif defined(USE_ALT_IMG)

	ImageLoader *d = new ImageLoader(); // delete of d is done some some lines down in this method
	assert(d!=NULL);

	int r = d->load_Image(filename);

	if (r>=0)
	{
		textureId = demoImageTextureFromImage(d);
	}
	else
	{
		printf("could not load image '%s'\n", filename);
	}

	if ((textureWidth!=NULL) && (textureHeight!=NULL))
	{
	 *textureWidth=d->imageWidth;
	 *textureHeight=d->imageHeight;
	}


	delete(d);

	#else
	#error
	#endif

	return textureId;

}

#if defined(USE_ALT_IMG)

ImageLoader::ImageLoader():
		buffer(NULL),
		imageWidth(0),
		imageHeight(0),
		imagePixels(NULL),
		imageSize(0),
		bytesPerPixel(0),
		iFormat(0),
		gFormat(0)
{
}

ImageLoader::~ImageLoader()
{
	if (buffer!=NULL)
	{
		delete(buffer);
	}

	// TODO if it was a PNG image do we need to free additional resources? free(imagePixels) It seems we shall do:
	// if(imagePixels != NULL) {
	//		delete[] imagePixels;
	//	}
	// See https://gist.github.com/roxlu/3077861
	// NOTE! DONT DO THIS IF IT WAS BMP "delete(buffer);" will delete the buffer.
	// Additionally if it was PNG then buffer can be deleted already in load_Image
}


int ImageLoader::load_Image(const char *filename)
{
	printf("load_Image: %s\n", filename);fflush(stdout);

	// http://stackoverflow.com/questions/18816126/c-read-the-whole-file-in-buffer
	std::ifstream filep(filename, std::ios::binary);
	filep.seekg(0, std::ios::end);
	std::streamsize fsize = filep.tellg();
	filep.seekg(0, std::ios::beg);

	if (fsize>0)
	{
		if (buffer!=NULL)
		{
			delete(buffer);
		}

		buffer = new std::vector<char>(fsize); // remember to delete in destructor
		assert(buffer!=NULL);

		printf("file '%s', size %d\n", filename, (int)fsize);

		filep.read(buffer->data(), fsize);

		const std::streamsize gcount = filep.gcount();

		filep.close();

		if (gcount==fsize)
		{

			unsigned char *ptr=(unsigned char *)buffer->data();

			/*const int is_png = !png_sig_cmp(ptr, 0, 8);
			if (is_png)
			{
				// http://www.libpng.org/pub/png/libpng-1.0.3-manual.html

				printf("can't use '%s', PNG is not yet supported\n", filename);
			}
			else*/ if ((gcount>28) && (ptr[0] == 'B') && (ptr[1] == 'M'))
			{
				// This is assumed to be an image in BMP fornat.
				// http://stackoverflow.com/questions/20595340/loading-a-tga-bmp-file-in-c-opengl

				const int BitsPerPixel = ptr[28];


				imageWidth = ptr[18] + (ptr[19] << 8);
				imageHeight =  ptr[22] + (ptr[23] << 8);
				imageSize = imageWidth * imageHeight;

				const int PixelsOffset = ptr[10] + (ptr[11] << 8);

				if (((imageSize * (BitsPerPixel/8)) + PixelsOffset) > gcount)
				{
					printf("wrong image size '%s'\n", filename);
					return -1;
				}

				imagePixels = ptr + PixelsOffset;


				if (BitsPerPixel == 24)
				{
					// TODO: There is probably a smarter way, like setting format in gluBuild2DMipmaps.
					for(int i = 0; i < imageSize ; ++i)
					{
					   const int index = i*3;
					   const unsigned char B = imagePixels[index];
					   const unsigned char R = imagePixels[index+2];

					   imagePixels[index] = R;
					   imagePixels[index+2] = B;
					}
					bytesPerPixel = 3;
					iFormat = GL_RGB8;
					gFormat = GL_RGB;
				}
				else if (BitsPerPixel == 32)
				{
					for(int i = 0; i < imageSize ; ++i)
					{
					   const int index = i*4;
					   const unsigned char A = imagePixels[index];
					   const unsigned char R = imagePixels[index+1];
					   const unsigned char G = imagePixels[index+2];
					   const unsigned char B = imagePixels[index+3];

					   imagePixels[index] = B;
					   imagePixels[index+1] = G;
					   imagePixels[index+2] = R;
					   imagePixels[index+3] = A;
					}
					bytesPerPixel = 4;
					iFormat = GL_RGBA; // http://stackoverflow.com/questions/6636847/odd-results-using-glteximage2d
					gFormat = GL_RGBA;
				}
				else
				{
					printf("Image '%s' has %d bits per pixel which is not supported\n", filename, BitsPerPixel);
					return -1;
				}
			}
			else
			{
				// This is assumed to be an image in PNG fornat.

				#ifdef USE_ALT_IMG_STB
				int comp=0;
				// TODO will STBI allocate a big enough buffer and give a pointer to it. Shall we do free on it? Or is it an internal buffer that we must copy?
				imagePixels = stbi_load_from_memory(ptr, gcount, &imageWidth, &imageHeight, &comp, STBI_rgb_alpha);
				imageSize = imageWidth * imageHeight;
				bytesPerPixel = 4;
				iFormat = GL_RGBA; // http://stackoverflow.com/questions/6636847/odd-results-using-glteximage2d
				gFormat = GL_RGBA;
				if (imagePixels==NULL)
				{
					printf("failed to decode image '%s'\n", filename);
					return -1;
				}
				else
				{
					printf("image loaded, comp %d\n",comp);
				}
				#else
				printf("image format not supported '%s'\n", filename);
				return -1;
				#endif

				// The following lines where added trying to figure out why all our textures are black on chrome. It did not help.
				if (buffer!=NULL)
				{
					delete(buffer);
				}
				buffer = new std::vector<char>(imageSize*bytesPerPixel);
				assert(buffer!=NULL);
				memcpy(buffer->data(), imagePixels, imageSize*bytesPerPixel);
				delete[] imagePixels;
				imagePixels = (unsigned char *)buffer->data();
				/*
				for(int i = 0; i < imageSize ; ++i)
				{
				   const int index = i*4;
				   const unsigned char B = imagePixels[index];
				   const unsigned char G = imagePixels[index+1];
				   const unsigned char R = imagePixels[index+2];
				   //const unsigned char A = imagePixels[index+3];

				   imagePixels[index] = B;
				   imagePixels[index+1] = G;
				   imagePixels[index+2] = R;
				   imagePixels[index+3] = 0x80;
				}
				*/

			}
		}
		else
		{
			printf("failed to read entire file '%s'\n", filename);
			return -1;
		}
	}
	else
	{
		printf("file not found or empty '%s'\n", filename);
		filep.close();
		return -1;
	}

	printf("image size %d %d\n", (int)imageWidth, (int)imageHeight);


	return 0;
}

#endif
