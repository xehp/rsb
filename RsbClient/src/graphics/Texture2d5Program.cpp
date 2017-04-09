/*
 * Texture25Program.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: May 4, 2015
 *      Author: henrik
 */



#include "includeGlEtc.h"
#include "CreateProgram.h"
#include "Texture2d5Program.h"

// Shader program for textured sprite objects

// http://en.wikipedia.org/wiki/Sprite_%28computer_graphics%29
// Billboard or 3D Sprite is a term often used to refer to sprites that are essentially texture mapped 3D facets that always have their surface normal facing into the camera.

// https://www.opengl.org/discussion_boards/showthread.php/182063-Drawing-a-billboard-sprite-from-a-single-point-need-geometry-shader

GLuint texture2d5ProgramId;
GLint texture2d5UniformMytexture;
GLint texture2d5Coord3dAttribute;
GLint texture2d5Texcoord_attribute; // texture2d5Texcoord_attribute
GLint texture2d5MatrixID;



int texture2d5InitGl()
{
	// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06

	/* init_resources */

#ifdef __APPLE__

	const GLchar* vertexShaderSource[] = { "\n attribute vec3 vertexPosition_modelspace; attribute vec2 texcoord; varying vec2 f_texcoord; uniform mat4 MVP; void main(){ gl_Position =  (MVP * vec4(0,0,0,1))+vec4(vertexPosition_modelspace.y, vertexPosition_modelspace.z, 0, 0); f_texcoord = texcoord;}" };
	const GLchar* fragmentShaderSource[] = { "\n varying vec2 f_texcoord; uniform sampler2D mytexture; void main(void) { gl_FragColor = texture2D(mytexture, f_texcoord); }" };

#else
	const GLchar* vertexShaderSource[] = { "#version 100\n attribute vec3 vertexPosition_modelspace; attribute vec2 texcoord; varying vec2 f_texcoord; uniform mat4 MVP; void main(){ gl_Position =  (MVP * vec4(0,0,0,1))+vec4(vertexPosition_modelspace.y, vertexPosition_modelspace.z, 0, 0); f_texcoord = texcoord;}" };
	//const GLchar* vertexShaderSource[] = { "#version 100\n attribute vec3 vertexPosition_modelspace; attribute vec2 texcoord; varying vec2 f_texcoord; uniform mat4 MVP; void main(){ gl_Position =  (MVP * vec4(vertexPosition_modelspace,1))+vec4(vertexPosition_modelspace.y, vertexPosition_modelspace.z, 0, 0); f_texcoord = texcoord;}" };
	//const GLchar* vertexShaderSource[] = { "#version 100\n attribute vec3 vertexPosition_modelspace; attribute vec2 texcoord; varying vec2 f_texcoord; uniform mat4 MVP; void main(){ gl_Position =  MVP * vec4(vertexPosition_modelspace,1); f_texcoord = texcoord;}" };
	const GLchar* fragmentShaderSource[] = { "#version 100\n precision mediump float; varying vec2 f_texcoord; uniform sampler2D mytexture; void main(void) { gl_FragColor = texture2D(mytexture, f_texcoord); }" };

#endif


	texture2d5ProgramId = createProgram(vertexShaderSource, fragmentShaderSource);





	texture2d5Coord3dAttribute = getAttribLocation(texture2d5ProgramId, "vertexPosition_modelspace");
	if (texture2d5Coord3dAttribute<0)
	{
		return -1;
	}


	texture2d5Texcoord_attribute = getAttribLocation(texture2d5ProgramId, "texcoord");
	if (texture2d5Texcoord_attribute<0)
	{
		return -1;
	}

	// Get a handle for our "MVP" uniform
	texture2d5MatrixID = getUniformLocation(texture2d5ProgramId, "MVP");

	texture2d5UniformMytexture = getUniformLocation(texture2d5ProgramId, "mytexture");
	if (texture2d5UniformMytexture<0)
	{
		return -1;
	}


	return 0;
}

