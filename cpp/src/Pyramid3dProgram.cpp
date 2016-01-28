/*
 * PyramidProgram.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 */

#include "includeGlEtc.h"

#include "CreateProgram.h"
#include "Pyramid3dProgram.h"

// Shader program for colored (not textured) 3D objects
GLuint pyramid3dProgramId = 0;
GLint pyramid3dMatrixID;
GLint pyramid3dFadeId; // http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
GLint pyramid3d_coordinates_attribute;
GLint pyramid3d_color_attribute;



int pyramid3dInitGl()
{

	// Get vertex source,
	#ifdef USE_OPENGL_2_1
		// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
		const GLchar* vertexShaderSource[] = { "#version 100\n attribute vec3 vertexPosition_modelspace; attribute vec3 model_color; varying vec3 f_color; uniform mat4 MVP; void main(){ gl_Position =  MVP * vec4(vertexPosition_modelspace,1); f_color = vec3(model_color.x, model_color.y, model_color.z); }" }; // yes, this works
	#else
		// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
		const GLchar* vertexShaderSource[] = { "#version 330 core\n layout(location = 0) in vec3 vertexPosition_modelspace; in vec3 model_color; uniform mat4 MVP; varying vec3 f_color; void main(){ gl_Position =  MVP * vec4(vertexPosition_modelspace,1); f_color = model_color;}" };
	#endif


	// Get fragment source
	#ifdef USE_OPENGL_2_1
		const GLchar* fragmentShaderSource[] = { "#version 100\n precision mediump float; uniform float fade; varying vec3 f_color; void main(void) { gl_FragColor = vec4(f_color.x, f_color.y, f_color.z, fade); }" }; // 1.0, 1.0, 1.0 for all white
	#else
		// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
		const GLchar* fragmentShaderSource[] = { "#version 330 core\n varying vec3 f_color; uniform float fade; out vec4 color; void main() {	color = vec4(f_color.x, f_color.y, f_color.z, fade); }" };
	#endif



	pyramid3dProgramId = createProgram(vertexShaderSource, fragmentShaderSource);



	pyramid3d_coordinates_attribute = getAttribLocation(pyramid3dProgramId, "vertexPosition_modelspace");
	if (pyramid3d_coordinates_attribute<0)
	{
		return -1;
	}

	pyramid3d_color_attribute = getAttribLocation(pyramid3dProgramId, "model_color");
	if (pyramid3d_color_attribute<0)
	{
		return -1;
	}

	pyramid3dFadeId = getUniformLocation(pyramid3dProgramId, "fade");
	if (pyramid3dFadeId == -1) {
		return -1;
	}


	// Get a handle for our "MVP" uniform
	pyramid3dMatrixID = getUniformLocation(pyramid3dProgramId, "MVP");




	return 0;
}
