/*
 * CubeProgram.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 *
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 */

#include "includeGlEtc.h"
#include "ImageLoader.h"
#include "CreateProgram.h"
#include "Cube3dProgram.h"

// Shader program for textured 3D objects
GLuint cube3dProgramId;
GLint cube3dUniformMytexture;
GLint cube3dCoord3dAttribute;
GLint cube3dTexcoord_attribute; // cube3dTexcoord_attribute
GLint cube3dMatrixID;
GLuint cube3dTextureId=ZERO_GL;

int cube3dtextureWidth=0;
int cube3dtextureHeight=0;

int cube3dProgramInitGl()
{
	// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06

	/* init_resources */


	const GLchar* vertexShaderSource[] = { "#version 100\n attribute vec3 vertexPosition_modelspace; attribute vec2 texcoord; varying vec2 f_texcoord; uniform mat4 MVP; void main(){ gl_Position =  MVP * vec4(vertexPosition_modelspace,1); f_texcoord = texcoord;}" };
	const GLchar* fragmentShaderSource[] = { "#version 100\n precision mediump float; varying vec2 f_texcoord; uniform sampler2D mytexture; void main(void) { gl_FragColor = texture2D(mytexture, f_texcoord); }" };
	//	const GLchar* vertexShaderSource[] = { "#version 100\n attribute vec3 vertexPosition_modelspace; uniform mat4 MVP; void main(){ gl_Position =  MVP * vec4(vertexPosition_modelspace,1); }" };
	//	const GLchar* fragmentShaderSource[] = { "#version 100\n precision mediump float; void main(void) { gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); }" }; // 1.0, 1.0, 1.0 for all white


	cube3dProgramId = createProgram(vertexShaderSource, fragmentShaderSource);





	cube3dCoord3dAttribute = getAttribLocation(cube3dProgramId, "vertexPosition_modelspace");
	if (cube3dCoord3dAttribute<0)
	{
		return -1;
	}


	cube3dTexcoord_attribute = getAttribLocation(cube3dProgramId, "texcoord");
	if (cube3dTexcoord_attribute<0)
	{
		return -1;
	}

	// Get a handle for our "MVP" uniform
	cube3dMatrixID = getUniformLocation(cube3dProgramId, "MVP");

	cube3dUniformMytexture = getUniformLocation(cube3dProgramId, "mytexture");
	if (cube3dUniformMytexture<0)
	{
		return -1;
	}



	if (cube3dTextureId==ZERO_GL)
	{
		#if defined(USE_IMG_LOAD) || defined(USE_ALT_IMG_STB)
		cube3dTextureId = ImageLoaderLoadTexture("townhall.png", &cube3dtextureWidth, &cube3dtextureHeight);
		#elif defined(USE_ALT_IMG)
		cube3dTextureId = ImageLoaderLoadTexture("townhall.bmp", &cube3dtextureWidth, &cube3dtextureHeight);
		#else
		#error
		#endif
	}

	return 0;
}

void cube3dCleanup()
{
	glDeleteTextures(1, &cube3dTextureId);
	glDeleteProgram(cube3dProgramId);
}
