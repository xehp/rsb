/*
 * Box2DProgram.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 */

#include "includeGlEtc.h"

#include "CreateProgram.h"
#include "Box2DProgram.h"




// Shader program for textured 2D objects
GLuint box2DProgramId;
GLint box2DTextureUniform;
GLint box2DCoord3dAttribute;
GLint box2DTexcoordAttribute;
GLint box2DMatrixID;




int mainInitGlBox2DTex()
{

#ifdef __APPLE__
	const GLchar* vertexShaderSource[] = { "\n attribute vec2 vertexPosition_modelspace; attribute vec2 texcoord; varying vec2 f_texcoord; void main(){ gl_Position =  vec4(vertexPosition_modelspace.x, vertexPosition_modelspace.y, 0, 1); f_texcoord = texcoord;}" };
	const GLchar* fragmentShaderSource[] = { "\n varying vec2 f_texcoord; uniform sampler2D boxTexture; void main(void) { gl_FragColor = texture2D(boxTexture, f_texcoord); }" };

#else
	const GLchar* vertexShaderSource[] = { "#version 100\n attribute vec2 vertexPosition_modelspace; attribute vec2 texcoord; varying vec2 f_texcoord; void main(){ gl_Position =  vec4(vertexPosition_modelspace.x, vertexPosition_modelspace.y, 0, 1); f_texcoord = texcoord;}" };
	const GLchar* fragmentShaderSource[] = { "#version 100\n precision mediump float; varying vec2 f_texcoord; uniform sampler2D boxTexture; void main(void) { gl_FragColor = texture2D(boxTexture, f_texcoord); }" };
#endif

	box2DProgramId = createProgram(vertexShaderSource, fragmentShaderSource);



	box2DCoord3dAttribute = getAttribLocation(box2DProgramId, "vertexPosition_modelspace");
	if (box2DCoord3dAttribute<0)
	{
		return -1;
	}

	box2DTexcoordAttribute = getAttribLocation(box2DProgramId, "texcoord");
	if (box2DTexcoordAttribute<0)
	{
		return -1;
	}

	box2DTextureUniform = getUniformLocation(box2DProgramId, "boxTexture");
	if (box2DTextureUniform<0)
	{
		return -1;
	}


	return 0;
}



