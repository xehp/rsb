/*
CrossHair.cpp

Copyright (C) 2015 Henrik Björkman www.eit.se/hb

History:
Created Jul 22, 2015 by Henrik Bjorkman www.eit.se





www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license



*/

#include "includeGlEtc.h"
#include "Quad2dProgram.h"


// #include <SOIL/SOIL.h> // http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06

#ifndef __EMSCRIPTEN__
#include "hca.h"
#endif

#include "HybridLib.h"
#include "WindowObject.h"
#include "Text2DBox.h"
#include "D3Vector.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"
#include "DrawText.h"
#include "ImageLoader.h"
#include "WordReader.h"
#include "MirrorBase.h"
#include "CrossHair.h"


/**
 * x is given in pixels while open GL uses floats where -1,-1 is one corner and 1,1 is the opposite corner.
 */

// Give width and height in pixels.
CrossHair::CrossHair(int width, int height):
			MirrorContainer(),
			crossVBO(ZERO_GL),
			crossIBO(ZERO_GL),
			width(width),
			height(height)
{
}

CrossHair::~CrossHair()
{
	glDeleteBuffers(1, &crossVBO);
	glDeleteBuffers(1, &crossIBO);
};


void CrossHair::renderAll()
{
	MirrorContainer::renderAll();
	// Read more at: http://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);



	glUseProgram( quad2dProgramId );





	// crosshairs

	glEnableVertexAttribArray(quad2dPosAttribLocation);
	glEnableVertexAttribArray(quad2dColorAttribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, crossVBO);
	glVertexAttribPointer(
		quad2dPosAttribLocation, // attribute
		2,                     // size, the two room coordinates x,y.
		GL_FLOAT,              // type
		GL_FALSE,              // normalized?
		5 * sizeof(GLfloat),   // stride, the two room coordinates and the three colors
		(void*)0               // array buffer offset
	);
	glVertexAttribPointer(
		quad2dColorAttribLocation,        // attribute
		3,                              // size, the three colors r,g,b
		GL_FLOAT,                       // type
		GL_FALSE,                       //  normalized?
		5 * sizeof(GLfloat),            // stride, the two room coordinates and the three colors
		(GLvoid*) (2 * sizeof(GLfloat)) // array buffer offset, skipping the two room coordinates x,y
	);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, crossIBO );
	//glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, NULL );
	glDrawElements( GL_LINES, 4, GL_UNSIGNED_SHORT, NULL );


	glDisableVertexAttribArray( quad2dPosAttribLocation );
	glDisableVertexAttribArray( quad2dColorAttribLocation );



};

void CrossHair::linkSelf(MirrorBase *parentPtr)
{
	MirrorContainer::linkSelf(parentPtr);

	WindowObject *windowObject = getWindowObject();
	assert(windowObject!=NULL);


	const float w2=windowObject->scale2dPixelToGlFloatX(width)/2;
	const float h2=windowObject->scale2dPixelToGlFloatY(height)/2;



	// crosshairs
	{
		// VBO data
		GLfloat vertexData[] = { -w2, 0.0f,      1.0f, 0.0f, 0.0f,
								 +w2, 0.0f,      0.0f, 1.0f, 0.0f,
								 0.0f, +h2,      0.0f, 0.0f, 1.0f,
								 0.0f, -h2,      1.0f, 1.0f, 1.0f
								};

		// IBO data
		//GLuint indexData[] = { 0, 1, 2, 3 };
		GLushort indexData[] = { 0, 1, 2, 3 };

		// Create VBO
		glGenBuffers( 1, &crossVBO );
		glBindBuffer( GL_ARRAY_BUFFER, crossVBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW );

		// Create IBO
		glGenBuffers( 1, &crossIBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, crossIBO );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW );
	}

}
