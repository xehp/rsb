/*
Text2DBox.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include "includeGlEtc.h"
#include "HybridLib.h"
#include "CreateProgram.h"
#include "Box2DProgram.h"

#ifndef __EMSCRIPTEN__
#include "hca.h"
#endif

#include "D3Vector.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"
#include "DrawText.h"
#include "ImageLoader.h"
#include "DrawImage.h"
#include "WordReader.h"
#include "MirrorContainer.h"

#include "Text2DBox.h"




Text2DBox::Text2DBox(float x, float y, float width, float height):
	MirrorContainer(),
	vbo_vertices(ZERO_GL),
	ibo_elements(ZERO_GL),
	vbo_texcoords(ZERO_GL),
	texture_id(ZERO_GL),
	x(x), // where on screen to show the text box, range (-1 .. +1)
	y(y), // -"-
	width(width), // size of the text box (range 0 .. 2)
	height(height),  // -"-
	//textString(NULL),
	buttonCommand(NULL) // If clicking in the text box shall be same as pressing a button, if so specify the command to send here.
{
	float w2 = width * 0.5f;
	float h2 = height * 0.5f;

	GLfloat box_vertices[] = {
	   x-w2,  y-h2,
	   x+w2,  y-h2,
	   x+w2,  y+h2,
       x-w2,  y+h2
	};


	printf("sizeof(box_vertices) %u\n", (unsigned int) sizeof(box_vertices));

	glUseProgram(box2DProgramId); // TODO is this line needed? Probably it is not needed here.


	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box_vertices), box_vertices, GL_STATIC_DRAW);

	// This probably tells where in the box that the texture shall be applied
#ifndef UPSIDE_DOWN_IMAGES
	GLfloat box_texcoords[] = {
	  // front
	  0.0, 1.0,
	  1.0, 1.0,
	  1.0, 0.0,
	  0.0, 0.0,
	};
#else
	GLfloat box_texcoords[] = {
	  // front
	  0.0, 0.0,
	  1.0, 0.0,
	  1.0, 1.0,
	  0.0, 1.0,
	};
#endif
	glGenBuffers(1, &vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box_texcoords), box_texcoords, GL_STATIC_DRAW);




	GLushort box_elements[] = {
		  // front
		   0,  1,  2,
		   2,  3,  0,
		};

	printf("sizeof(box_elements) %u\n", (unsigned int)sizeof(box_elements));

	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(box_elements), box_elements, GL_STATIC_DRAW);



	// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06


};


Text2DBox::~Text2DBox()
{
	clearText();

	//glUseProgram(box2DProgramId); // TODO is this line needed? Probably it is not needed here.

	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_texcoords);
	glDeleteBuffers(1, &ibo_elements);

	//free(textString);
	//free(buttonCommand);
};


void Text2DBox::renderAll()
{
	MirrorContainer::renderAll();

	// Is there a texture to show?
	if (texture_id != ZERO_GL)
	{


		glUseProgram(box2DProgramId);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glUniform1i(box2DTextureUniform, /*GL_TEXTURE*/0);


		glEnableVertexAttribArray(box2DCoord3dAttribute);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(
			box2DCoord3dAttribute, // attribute
			2,                     // size, the two room coordinates x,y.
			GL_FLOAT,              // type
			GL_FALSE,              // normalized? No, take our values as-is.
			0,                     // stride
			(void*)0               // array buffer offset, offset of first element
		);

		glEnableVertexAttribArray(box2DTexcoordAttribute);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glVertexAttribPointer(
			box2DTexcoordAttribute, // attribute
			2,                  // number of elements per vertex, here (x,y)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			0,                  // no extra data between each position
			0                   // offset of first element
		  );



		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);

		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);


		glDisableVertexAttribArray(box2DCoord3dAttribute);
		glDisableVertexAttribArray(box2DTexcoordAttribute);
	}
};

// User can call this to put or change the text in the text box
void Text2DBox::setText(const char *str)
{
	//glUseProgram(box2DProgramId); // TODO is this line needed? Probably it is not needed here.

	clearText();

	const unsigned int characterBoxSize = (width * 2 * mainAspectRatio) / height; // How many characters that there is room for in one line in the box without squeezing the characters.

	//free(textString);
	//textString=stringCopyFactory(str);


	if ((str!=NULL) && (*str))
	{
		texture_id = drawTextTextureFactory(str, characterBoxSize); // This creates an image with the desired text. Remember to delete texture (glDeleteTextures) when no longer needed.
	}

}

void Text2DBox::setButtonCommand(const char *str)
{
	free(buttonCommand);
	buttonCommand=NULL;
	if (str!=NULL)
	{
		buttonCommand=stringCopyFactory(str);
	}
}

// Call this to remove text in a text box
void Text2DBox::clearText()
{
	//glUseProgram(box2DProgramId); // TODO is this line needed? Probably it is not needed here.

	if (texture_id!=ZERO_GL)
	{
		glDeleteTextures(1, &texture_id);
		texture_id = ZERO_GL;
	}

	//free(textString);
	//textString=NULL;
	free(buttonCommand);
	buttonCommand=NULL;
}



// User can call this to put or change the text in the text box
void Text2DBox::setImage(int xOffset, int yOffset, int width, int height)
{
	//glUseProgram(box2DProgramId); // TODO is this line needed? Probably it is not needed here.

	clearText();

	texture_id = drawImageTextureFactory(xOffset, yOffset, width, height); // This creates an image, Remember to delete texture (glDeleteTextures) when no longer needed.
}
