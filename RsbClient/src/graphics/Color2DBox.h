/*
Color2DBox.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef COLOR2DBOX_H_
#define COLOR2DBOX_H_

#include "MirrorContainer.h"

class Color2DBox : public MirrorContainer
{
public:
	Color2DBox(float x, float y, float width, float height);

	~Color2DBox();


	virtual void renderAll();
	virtual void changeSize(float x, float y, float width, float height);


	GLuint quadVBO;
	GLuint quadIBO;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	Color2DBox(const Color2DBox&);
	Color2DBox& operator=(const Color2DBox&);

};
#endif

