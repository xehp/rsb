/*
 * YukigassenTexturedRoom.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: Apr 15, 2015
 *      Author: henrik
 */

#include "includeGlEtc.h"
#include "HybridLib.h"
#include "Cube3dProgram.h"
#include "ImageLoader.h"
#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "YukigassenTexturedRoom.h"

GLuint YukigassenTexturedRoom::texture_id=ZERO_GL; // The GL texture


YukigassenTexturedRoom::YukigassenTexturedRoom() :
	YukigassenRoom(),
	vbo_vertices(ZERO_GL),
	ibo_elements(ZERO_GL),
	vbo_texcoords(ZERO_GL),
	//texture_id(ZERO_GL),
	textureIndex(0),
	prevTextureIndex(0)
{
}


YukigassenTexturedRoom::~YukigassenTexturedRoom() {
	//glDeleteTextures(1, &texture_id); // as long as texture_id is static don't delete here
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_texcoords);
	glDeleteBuffers(1, &ibo_elements);
}


void YukigassenTexturedRoom::readSelf(WordReader *wr)
{
	YukigassenRoom::readSelf(wr);

	textureIndex = wr->readInt();

/*
	if (textureIndex!=prevTextureIndex)
	{
		setupInternalRenderingData();
		prevTextureIndex = textureIndex;
	}
*/
}

#ifndef __EMSCRIPTEN__
void YukigassenTexturedRoom::writeSelf(WordWriter *ww)
{
	YukigassenRoom::writeSelf(ww);

   	ww->writeInt(textureIndex);
}
#endif



void YukigassenTexturedRoom::setupInternalRenderingData()
{

	const GLfloat x = roomSize.x/2.0f;
	const GLfloat y = roomSize.y/2.0f;
	const GLfloat z = roomSize.z/2.0f;

	/*
	           y
	           ^
	           |
	           |
	           |
	       v6--------v5
	       /         /|
	      / |       / |
	     /         /  |
	   v1--------v0   |   ------> x
	    |  v7  -  |  v4
	    |         |  /
	    | /       | /
	    |         |/
	   v2--------v3
	      /
	     /
	    /
	   z
	*/


	GLfloat cube_vertices[] = {

	  // front, surface 1
	   x,  y,  z,   //  0 v0 a
	   x, -y,  z,   //  1 v3 d
	   x, -y, -z,   //  2 v4 h
	   x,  y, -z,   //  3 v5 e

	  // right, surface 2
	  -x,  y,  z,   //  4 v1 b
	   x,  y,  z,   //  5 v0 a
	   x,  y, -z,   //  6 v5 e
	  -x,  y, -z,   //  7 v6 f

	  // bottom, surface 3
	  -x,  y,  z,   //  8 v1 b
	  -x, -y,  z,   //  9 v2 c
	   x, -y,  z,   // 10 v3 d
	   x,  y,  z,   // 11 v0 a

	  // top, surface 4
	   x,  y, -z,   // 12 v5 e
	   x, -y, -z,   // 13 v4 h
	  -x, -y, -z,   // 14 v7 g
	  -x,  y, -z,   // 15 v6 f

	  // left, surface 5
	   x, -y,  z,   // 16 v3 d
	  -x, -y,  z,   // 17 v2 c
	  -x, -y, -z,   // 18 v7 g
	   x, -y, -z,   // 19 v4 h

	  // back, surface 6
	  -x, -y,  z,   // 20 v2 c
	  -x,  y,  z,   // 21 v1 b
	  -x,  y, -z,   // 22 v6 f
	  -x, -y, -z,   // 23 v7 g
	};



	//printf("sizeof(cube_vertices) %u\n", (unsigned int) sizeof(cube_vertices));

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

#if 0
	// Use same texture on all sides
	GLfloat cube_texcoords[2*4*6] = {
	  // front
	  0.0, 1.0,
	  1.0, 1.0,
	  1.0, 0.0,
	  0.0, 0.0,
	};
	for (int i = 1; i < 6; i++) memcpy(&cube_texcoords[i*4*2], &cube_texcoords[0], 2*4*sizeof(GLfloat));
#else
	//const float yOffset=0.0f;
	//const float ySize=1.0f/8.0f;
	//const float yEnd=yOffset+ySize;

	const float nTexturesX = cube3dtextureWidth/32.0f; // How many textures in X
	const float nTexturesY = cube3dtextureHeight/32.0f; // How many textures in Y


	const float yOffset=textureIndex/nTexturesY;
	const float ySize=1.0f/nTexturesY;
	const float yEnd=yOffset+ySize;

	// Use different part of texture for each side, it seems these here shall match cube_vertices.
	GLfloat cube_texcoords[2*4*6] = {

	  // xp = s0 (front)
	  0.0f,           yEnd,
	  1.0f/nTexturesX, yEnd,
	  1.0f/nTexturesX, yOffset,
	  0.0f,           yOffset,

	  // yp = s1 (right)
	  1.0f/nTexturesX, yEnd,
	  2.0f/nTexturesX, yEnd,
	  2.0f/nTexturesX, yOffset,
	  1.0f/nTexturesX, yOffset,

	  // zp = s2 (bottom)
	  2.0f/nTexturesX, yEnd,
	  3.0f/nTexturesX, yEnd,
	  3.0f/nTexturesX, yOffset,
	  2.0f/nTexturesX, yOffset,

	  // zn = s3 (top)
	  3.0f/nTexturesX, yEnd,
	  4.0f/nTexturesX, yEnd,
	  4.0f/nTexturesX, yOffset,
	  3.0f/nTexturesX, yOffset,

	  // yn = s4 (left)
	  4.0f/nTexturesX, yEnd,
	  5.0f/nTexturesX, yEnd,
	  5.0f/nTexturesX, yOffset,
	  4.0f/nTexturesX, yOffset,

	  // xn = s5 (rear)
	  5.0f/nTexturesX, yEnd,
	  6.0f/nTexturesX, yEnd,
	  6.0f/nTexturesX, yOffset,
	  5.0f/nTexturesX, yOffset,
	};
#endif

	glGenBuffers(1, &vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);



	GLushort cube_elements[] = {
		  // xp = s0 (front)
		   0,  1,  2,   // v0 v3 v4
		   2,  3,  0,   // v4 v5 v0
		  // yp = s1 (right)
		   4,  5,  6,   // v1 v0 v5
		   6,  7,  4,   // v5 v6 v1
		  // zp = s2 (bottom)
		   8,  9, 10,   // v1 v2 v3
		  10, 11,  8,   // v3 v0 v1
		  // zn = s3 (top)
		  12, 13, 14,   // v5 v4 v7
		  14, 15, 12,   // v7 v6 v5
		  // yn = s4 (left)
		  16, 17, 18,   // v3 v2 v7
		  18, 19, 16,   // v7 v4 v3
		  // xn = s5 (rear)
		  20, 21, 22,   // v2 v1 v6
		  22, 23, 20,   // v6 v7 v2
		};

	//printf("sizeof(cube_elements) %u\n", (unsigned int)sizeof(cube_elements));

	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);



	if (texture_id==ZERO_GL)
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


void YukigassenTexturedRoom::renderSelf()
{

	/*if (this->id==ourAvatarId)
	{
		// Don't render own avatar (we would only see ourselves from inside).
		// Instead render a selection box (hands sort of)

		return;
	}*/


	glUseProgram(cube3dProgramId);

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);


	// Tell GL to use the new translation matrix
	glUniformMatrix4fv(cube3dMatrixID, 1, GL_FALSE, &MVP[0][0]);






	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(cube3dUniformMytexture, /*GL_TEXTURE*/0);


	glEnableVertexAttribArray(cube3dCoord3dAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glVertexAttribPointer(
		cube3dCoord3dAttribute, // attribute
		3,                     // size, the three room coordinates x,y,z.
		GL_FLOAT,              // type
		GL_FALSE,              // normalized? No, take our values as-is.
		3 * sizeof(GLfloat),   // stride
		(void*)0               // array buffer offset, offset of first element
	);

	glEnableVertexAttribArray(cube3dTexcoord_attribute);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glVertexAttribPointer(
		cube3dTexcoord_attribute, // attribute
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


	glDisableVertexAttribArray(cube3dCoord3dAttribute);
	glDisableVertexAttribArray(cube3dUniformMytexture);

};


void YukigassenTexturedRoomCleanup()
{
	glDeleteTextures(1, &YukigassenTexturedRoom::texture_id);
}
