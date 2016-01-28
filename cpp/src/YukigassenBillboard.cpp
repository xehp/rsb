/*
 * YukigassenBillboard.cpp
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 17, 2015
 *      Author: henrik
 */

#include "includeGlEtc.h"
#include "Texture2d5Program.h"
#include "Cube3dProgram.h"
#include "ImageLoader.h"
#include "WordReader.h"
#include "YukigassenBillboard.h"

GLuint YukigassenBillboard::texture_id=0;


YukigassenBillboard::YukigassenBillboard() :
	YukigassenProperty(),
	vbo_vertices(0),
	ibo_elements(0),
	vbo_texcoords(0),
	textureIndex(0),
	//int prevTextureIndex;
	entityColor(0)
{
}

YukigassenBillboard::~YukigassenBillboard() {
	//glDeleteTextures(1, &texture_id); // as long as texture_id is static don't delete here
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_texcoords);
	glDeleteBuffers(1, &ibo_elements);
}

void YukigassenBillboard::readSelf(WordReader *wr)
{
	YukigassenProperty::readSelf(wr);

	textureIndex = wr->readInt();
	entityColor = wr->readInt();
}



void YukigassenBillboard::setupInternalRenderingData()
{

	//const GLfloat x = roomSize.x/2.0f;
	const GLfloat y = roomSize.y/2.0f;
	const GLfloat z = roomSize.z/2.0f*mainAspectRatio;

	GLfloat cube_vertices[] = {
	  // front, surface 1
	   0.0f,  y,  z,
	   0.0f, -y,  z,
	   0.0f, -y, -z,
	   0.0f,  y, -z,
	};




	//printf("sizeof(cube_vertices) %u\n", (unsigned int) sizeof(cube_vertices));

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	const float nTexturesX = cube3dtextureWidth/32.0f; // How many textures in X
	const float nTexturesY = cube3dtextureHeight/32.0f; // How many textures in Y

	const float xSize=1.0f/nTexturesX;
	const float xOffset=(nTexturesX-1)*xSize;
	const float ySize=1.0f/nTexturesY;
	const float yOffset=textureIndex*ySize;

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
		#if defined(USE_IMG_LOAD) || defined(USE_ALT_IMG_STB)
		texture_id = ImageLoaderLoadTexture("townhall.png", NULL, NULL);
		#elif defined(USE_ALT_IMG)
		texture_id = ImageLoaderLoadTexture("townhall.bmp", NULL, NULL);
		#else
		#error
		#endif
	}

};


void YukigassenBillboard::renderSelf()
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


#if 0

	// Translation matrix.
	glm::mat4 cubeModelT = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			pao.pos.x, pao.pos.y, pao.pos.z, 1.0f
	);

	D3Vector rightSide(pao.nose, pao.head);


/*
	glm::mat4 cubeModelR = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
		         0.0f,             0.0f,       0.0f, 1.0f
	);*/

	// Rotation matrix, this is not tested! (it does rotate but is it correctly rotated?)
	glm::mat4 cubeModelR = glm::mat4(
		 	pao.nose.x,   pao.nose.y,  pao.nose.z,  0.0f,
			rightSide.x, rightSide.y, rightSide.z, 0.0f,
			-pao.head.x, -pao.head.y, -pao.head.z,  0.0f,
		         0.0f,             0.0f,       0.0f, 1.0f
	);

	glm::mat4 cubeModel = cubeModelT * cubeModelR;

	glm::mat4 MVP = Projection * View * cubeModel; // Remember, matrix multiplication is the other way around

#endif

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
