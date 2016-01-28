/*

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

*/

#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H



int imageLoaderInit(); // This must be called before ImageLoader is used
void imageLoaderQuit(); // This must be called at program exit if ImageLoaderInit has been called.

GLuint ImageLoaderLoadTexture(const char *filename,int *textw,int *texth);


// Own configuration choices

// Uncomment one of these
//#define USE_IMG_LOAD
#define USE_ALT_IMG

// Activate PNG support for USE_ALT_IMG
#define USE_ALT_IMG_STB



#ifdef USE_ALT_IMG

#include <vector>

class ImageLoader
{

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	ImageLoader(const ImageLoader&);
	ImageLoader& operator=(const ImageLoader&);

public:
	ImageLoader();
	~ImageLoader();

	int load_Image(const char *filename);


	std::vector<char> *buffer;

	int imageWidth;
	int imageHeight;
	unsigned char *imagePixels;
	int imageSize;
	int bytesPerPixel;
	int iFormat;
	int gFormat;

};

#endif


#endif
