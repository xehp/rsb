/*
ColorPyramid.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include "MirrorContainer.h"

class ColorPyramid : public MirrorContainer
{
public:
	ColorPyramid(float x, float y, float z);

	~ColorPyramid();

	virtual void renderSelf();

	GLuint pyramidVertexBufferId;
	GLuint pyramidAxisVertexBufferId; // ID/Name, it is called object name in OpenGl but they are numbers so for me its an ID.


	float pyramidX;
	float pyramidY;
	float pyramidZ;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	ColorPyramid(const ColorPyramid&);
	ColorPyramid& operator=(const ColorPyramid&);

};
