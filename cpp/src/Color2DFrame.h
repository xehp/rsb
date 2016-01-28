/*
Color2DFrame.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef COLOR2DFRAME_H_
#define COLOR2DFRAME_H_

#include "MirrorContainer.h"

class Color2DFrame : public MirrorContainer
{
public:
	Color2DFrame(float x, float y, float width, float height);

	~Color2DFrame();


	virtual void renderAll();
	virtual void changeSize(float x, float y, float width, float height);


	GLuint quadVBO;
	GLuint quadIBO;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	Color2DFrame(const Color2DFrame&);
	Color2DFrame& operator=(const Color2DFrame&);

};
#endif
