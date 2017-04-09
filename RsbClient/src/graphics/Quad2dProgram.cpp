/*
 * QuadProgram.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 */

#include "includeGlEtc.h"
#include "CreateProgram.h"
#include "Quad2dProgram.h"

// Shader program for colored (not textured) 2D objects
GLuint quad2dProgramId = 0;
GLint quad2dPosAttribLocation = -1;
GLint quad2dColorAttribLocation = -1;

int quad2dInitGl()
{
#ifdef __APPLE__
	const GLchar* vertexShaderSource[] = { "\n attribute vec2 vertexPosition_modelspace; attribute vec3 model_color; varying vec3 f_color; void main(){ gl_Position =  vec4(vertexPosition_modelspace.x, vertexPosition_modelspace.y, 0, 1); f_color = vec3(model_color.x, model_color.y, model_color.z); }" };
	const GLchar* fragmentShaderSource[] = { "\n /*uniform float fade;*/ varying vec3 f_color; void main(void) { gl_FragColor = vec4(f_color.x, f_color.y, f_color.z, 1.0); }" }; // 1.0, 1.0, 1.0 for all white

#else
	const GLchar* vertexShaderSource[] = { "#version 100\n attribute vec2 vertexPosition_modelspace; attribute vec3 model_color; varying vec3 f_color; void main(){ gl_Position =  vec4(vertexPosition_modelspace.x, vertexPosition_modelspace.y, 0, 1); f_color = vec3(model_color.x, model_color.y, model_color.z); }" };
	const GLchar* fragmentShaderSource[] = { "#version 100\n precision mediump float; /*uniform float fade;*/ varying vec3 f_color; void main(void) { gl_FragColor = vec4(f_color.x, f_color.y, f_color.z, 1.0); }" }; // 1.0, 1.0, 1.0 for all white
#endif

	quad2dProgramId = createProgram(vertexShaderSource, fragmentShaderSource);


	quad2dPosAttribLocation = getAttribLocation(quad2dProgramId, "vertexPosition_modelspace");
	if (quad2dPosAttribLocation == -1) { return -1;}

	quad2dColorAttribLocation = getAttribLocation(quad2dProgramId, "model_color");
	if (quad2dPosAttribLocation == -1) { return -1;}



	return 0;
}

