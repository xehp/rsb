/*
TexCube.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/
#if 0
#include "MirrorContainer.h"


class TexCube : public MirrorContainer
{
public:
	TexCube(float x, float y, float z);

	~TexCube();

	virtual void renderSelf();


	GLuint vbo_vertices;
	GLuint ibo_elements;
	GLuint vbo_texcoords;
	//static GLuint texture_id;

	float cubeX;
	float cubeY;
	float cubeZ;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	TexCube(const TexCube&);
	TexCube& operator=(const TexCube&);

};


void TexCubeCleanup();

#endif
