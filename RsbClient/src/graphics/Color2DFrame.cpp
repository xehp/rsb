/*
Color2DFrame.cpp

Copyright (C) 2015 Henrik Björkman www.eit.se/hb

History:
Created 2015 by Henrik Bjorkman www.eit.se





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
#include "D3Vector.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"
#include "DrawText.h"
#include "ImageLoader.h"
#include "WordReader.h"
#include "MirrorBase.h"
#include "Color2DFrame.h"



Color2DFrame::Color2DFrame(float x, float y, float width, float height):
	MirrorContainer(),
	quadVBO(ZERO_GL),
	quadIBO(ZERO_GL)
{
	changeSize(x, y, width, height);
};

Color2DFrame::~Color2DFrame()
{
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &quadIBO);
};


void Color2DFrame::changeSize(float x, float y, float width, float height)
{
	if (quadVBO!=ZERO_GL)
	{
		glDeleteBuffers(1, &quadVBO);
		glDeleteBuffers(1, &quadIBO);
		quadVBO=ZERO_GL;
		quadIBO=ZERO_GL;
	}


	const float w2=width/2;
	const float h2=height/2;

	// The colored box
	{
		// VBO data
		GLfloat vertexData[] = { x-w2, y-h2,      1.0f, 0.0f, 0.0f,
								 x+w2, y-h2,      0.0f, 1.0f, 0.0f,
								 x+w2, y+h2,      0.0f, 0.0f, 1.0f,
								 x-w2, y+h2,      1.0f, 1.0f, 1.0f
								};

		// IBO data
		//GLuint indexData[] = { 0, 1, 2, 3 };
		GLushort indexData[] = { 0, 1, 2, 3, 1, 2, 0, 3 };

		// Create VBO
		glGenBuffers( 1, &quadVBO );
		glBindBuffer( GL_ARRAY_BUFFER, quadVBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW );

		// Create IBO
		glGenBuffers( 1, &quadIBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, quadIBO );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW );
	}


};


void Color2DFrame::renderAll()
{
	MirrorContainer::renderAll();
	// Read more at: http://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);


	glUseProgram( quad2dProgramId );


	glEnableVertexAttribArray(quad2dPosAttribLocation);
	glEnableVertexAttribArray(quad2dColorAttribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
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

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, quadIBO );
	glDrawElements( GL_LINES, 8, GL_UNSIGNED_SHORT, NULL ); // second argument here is number of elements in indexData


	glDisableVertexAttribArray( quad2dPosAttribLocation );
	glDisableVertexAttribArray( quad2dColorAttribLocation );

};
