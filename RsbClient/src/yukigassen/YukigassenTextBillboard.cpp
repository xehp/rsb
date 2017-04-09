/*
 * YukigassenTextBillboard.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: May 7, 2015
 *      Author: henrik
 */

#include "includeGlEtc.h"
#include "WindowObject.h"
#include "Texture2d5Program.h"
#include "Cube3dProgram.h"
#include "ImageLoader.h"
#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "DrawText.h"
#include "YukigassenTextBillboard.h"


YukigassenTextBillboard::YukigassenTextBillboard(float width, float height) :
	YukigassenProperty(),
	width(width), // size of the text box (range 0 .. 2)
	height(height),  // -"-
	vbo_vertices(0),
	ibo_elements(0),
	vbo_texcoords(0),
	texture_id(0),
	textureIndex(0),
	entityColor(0)
{
	drawText=drawTextSmall;
}

YukigassenTextBillboard::~YukigassenTextBillboard() {
	glDeleteTextures(1, &texture_id);
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_texcoords);
	glDeleteBuffers(1, &ibo_elements);
}

void YukigassenTextBillboard::readSelf(WordReader *wr)
{
	YukigassenProperty::readSelf(wr);

	textureIndex = wr->readInt();
	entityColor = wr->readInt();
}

#ifndef __EMSCRIPTEN__
void YukigassenTextBillboard::writeSelf(WordWriter *ww)
{
	YukigassenProperty::writeSelf(ww);

   	ww->writeInt(textureIndex);
   	ww->writeInt(entityColor);
}
#endif


void YukigassenTextBillboard::setupInternalRenderingData()
{
	WindowObject *windowObject=getWindowObject();
	assert(windowObject!=NULL);

	glUseProgram(texture2d5ProgramId); // This line is probably not needed here.

	//const GLfloat x = roomSize.x/2.0f;
	const GLfloat y = width;
	const GLfloat z = height*windowObject->mainAspectRatio();

	GLfloat cube_vertices[] = {
	  // front, surface 1
	   0.0f, -y, -z,
	   0.0f,  y, -z,
	   0.0f,  y,  z,
	   0.0f, -y,  z,
	};




	//printf("sizeof(cube_vertices) %u\n", (unsigned int) sizeof(cube_vertices));

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	const float xSize=1.0f;
	const float xOffset=0.0f;
	const float ySize=1.0f;
	const float yOffset=0.0f;

	// Use different part of texture for each side
	GLfloat cube_texcoords[] = {
	  // front
	  xOffset, yOffset+ySize,
	  xOffset+xSize, yOffset+ySize,
	  xOffset+xSize, yOffset,
	  xOffset, yOffset,
	};

	glGenBuffers(1, &vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);




	GLushort cube_elements[] = {
		  // front
		   0,  1,  2,
		   2,  3,  0,
		};

	//printf("sizeof(cube_elements) %u\n", (unsigned int)sizeof(cube_elements));

	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);



	if (texture_id==0)
	{
		//texture_id = ImageLoaderLoadTexture("townhall.png", NULL, NULL);

		texture_id = drawText->textureFactory("Avatar", 2); // This creates an image with the desired text. Remember to delete texture (glDeleteTextures) when no longer needed.
	}

};


void YukigassenTextBillboard::renderSelf()
{

	/*if (this->id==ourAvatarId)
	{
		// Don't render own avatar (we would only see ourselves from inside).
		// Instead render a selection box (hands sort of)

		return;
	}*/


	glUseProgram(texture2d5ProgramId);


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);



	// Tell GL to use the new translation matrix
	glUniformMatrix4fv(texture2d5MatrixID, 1, GL_FALSE, &MVP[0][0]);






	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(texture2d5UniformMytexture, /*GL_TEXTURE*/0);


	glEnableVertexAttribArray(texture2d5Coord3dAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glVertexAttribPointer(
		texture2d5Coord3dAttribute, // attribute
		3,                     // size, the three room coordinates x,y,z.
		GL_FLOAT,              // type
		GL_FALSE,              // normalized? No, take our values as-is.
		3 * sizeof(GLfloat),   // stride
		(void*)0               // array buffer offset, offset of first element
	);

	glEnableVertexAttribArray(texture2d5Texcoord_attribute);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glVertexAttribPointer(
		texture2d5Texcoord_attribute, // attribute
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


	glDisableVertexAttribArray(texture2d5Coord3dAttribute);
	glDisableVertexAttribArray(texture2d5UniformMytexture);

};

// Call this to remove text in a text box
void YukigassenTextBillboard::clearText()
{
	if (texture_id!=ZERO_GL)
	{
		glDeleteTextures(1, &texture_id);
		texture_id = ZERO_GL;
	}
}

// User can call this to put or change the text in the text box
void YukigassenTextBillboard::setText(const char *str)
{
	WindowObject *windowObject=getWindowObject();
	assert(windowObject!=NULL);

	clearText();

	const unsigned int len = strlen(str);
	//const unsigned int characterBoxSize = (width * 2 * mainAspectRatio) / height; // How many characters that there is room for in one line in the box without squeezing the characters.


	if ((str!=NULL) && (*str))
	{
		texture_id = drawText->textureFactory(str, len); // This creates an image with the desired text. Remember to delete texture (glDeleteTextures) when no longer needed.
	}

	width = len * height / (2.0 * windowObject->mainAspectRatio());

	setupInternalRenderingData();
}
