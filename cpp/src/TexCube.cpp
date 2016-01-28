/*
TexCube.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/
#if 0

#include "includeGlEtc.h"
#include "HybridLib.h"
#include "Cube3dProgram.h"


// #include <SOIL/SOIL.h> // http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06



#ifndef __EMSCRIPTEN__
#include "hca.h"
#endif

#include "D3Vector.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"
#include "DrawText.h"
#include "ImageLoader.h"
#include "WordReader.h"

#include "TexCube.h"

//GLuint TexCube::texture_id=ZERO_GL;

// Not sure this class is used or even tested.

TexCube::TexCube(float x, float y, float z):
	MirrorContainer(),
	vbo_vertices(ZERO_GL),
	ibo_elements(ZERO_GL),
	vbo_texcoords(ZERO_GL),
	//texture_id(ZERO_GL),
	cubeX(x),
	cubeY(y),
	cubeZ(z)
{
	// This is the corners of the cube or cuboid to be rendered.
	GLfloat cube_vertices[] = {
	  // front
	  -1.0, -1.0,  1.0,
	   1.0, -1.0,  1.0,
	   1.0,  1.0,  1.0,
	  -1.0,  1.0,  1.0,
	  // top
	  -1.0,  1.0,  1.0,
	   1.0,  1.0,  1.0,
	   1.0,  1.0, -1.0,
	  -1.0,  1.0, -1.0,
	  // back
	   1.0, -1.0, -1.0,
	  -1.0, -1.0, -1.0,
	  -1.0,  1.0, -1.0,
	   1.0,  1.0, -1.0,
	  // bottom
	  -1.0, -1.0, -1.0,
	   1.0, -1.0, -1.0,
	   1.0, -1.0,  1.0,
	  -1.0, -1.0,  1.0,
	  // left
	  -1.0, -1.0, -1.0,
	  -1.0, -1.0,  1.0,
	  -1.0,  1.0,  1.0,
	  -1.0,  1.0, -1.0,
	  // right
	   1.0, -1.0,  1.0,
	   1.0, -1.0, -1.0,
	   1.0,  1.0, -1.0,
	   1.0,  1.0,  1.0,
	};

	//printf("sizeof(cube_vertices) %u\n", (unsigned int) sizeof(cube_vertices));

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);


	// This will tell which part of the texture to use
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
	// Use different part of texture for each side
	GLfloat cube_texcoords[2*4*6] = {
	  // front
	  0.0, 1.0,
	  1.0/8, 1.0,
	  1.0/8, 0.0,
	  0.0, 0.0,

	  1.0/8, 1.0,
	  2.0/8, 1.0,
	  2.0/8, 0.0,
	  1.0/8, 0.0,

	  2.0/8, 1.0,
	  3.0/8, 1.0,
	  3.0/8, 0.0,
	  2.0/8, 0.0,

	  3.0/8, 1.0,
	  4.0/8, 1.0,
	  4.0/8, 0.0,
	  3.0/8, 0.0,

	  4.0/8, 1.0,
	  5.0/8, 1.0,
	  5.0/8, 0.0,
	  4.0/8, 0.0,

	  5.0/8, 1.0,
	  6.0/8, 1.0,
	  6.0/8, 0.0,
	  5.0/8, 0.0,
	};
#endif


	glGenBuffers(1, &vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);


#ifndef DONT_USE_IBO
	// This will tell in what order the vertices in cube_vertices shall be used.
	GLushort cube_elements[] = {
		  // front
		   0,  1,  2,
		   2,  3,  0,
		  // top
		   4,  5,  6,
		   6,  7,  4,
		  // back
		   8,  9, 10,
		  10, 11,  8,
		  // bottom
		  12, 13, 14,
		  14, 15, 12,
		  // left
		  16, 17, 18,
		  18, 19, 16,
		  // right
		  20, 21, 22,
		  22, 23, 20,
		};

	//printf("sizeof(cube_elements) %u\n", (unsigned int)sizeof(cube_elements));

	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
#endif

/*
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
*/
};

TexCube::~TexCube()
{
	//glDeleteTextures(1, &texture_id); // as long as texture_id is static don't delete here, do it in cleanup instead.
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_texcoords);
	glDeleteBuffers(1, &ibo_elements);
};


void TexCube::renderSelf()
{
	//MirrorContainer::renderAll();

	// Read more at: https://gitlab.com/wikibooks-opengl/modern-tutorials/blob/master/tut06_textures/cube.cpp


	glUseProgram(cube3dProgramId);


	// Tell GL to use the new translation matrix
	glUniformMatrix4fv(cube3dMatrixID, 1, GL_FALSE, &MVP[0][0]);

	glEnable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture_id);
	glBindTexture(GL_TEXTURE_2D, cube3dTextureId);
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

	glEnableVertexAttribArray(cube3dCoord3dAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glVertexAttribPointer(
		cube3dCoord3dAttribute, // attribute
	    2,                  // number of elements per vertex, here (x,y)
	    GL_FLOAT,           // the type of each element
	    GL_FALSE,           // take our values as-is
	    0,                  // no extra data between each position
	    0                   // offset of first element
	  );

#ifndef DONT_USE_IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);

	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
#else
	//glDrawArrays(GL_TRIANGLES, 0, 18);
	glDrawArrays(GL_TRIANGLES, 0, 6);
#endif

	glDisableVertexAttribArray(cube3dCoord3dAttribute);
	glDisableVertexAttribArray(cube3dUniformMytexture);

};

void TexCubeCleanup()
{
	//glDeleteTextures(1, &TexCube::texture_id);
}
#endif
