/*
 * CreateProgram.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 *
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
#include "CreateProgram.h"

//Screen dimension constants
const int MAIN_SCREEN_WIDTH = 800;
const int MAIN_SCREEN_HEIGHT = 600;

const float mainAspectRatio = (MAIN_SCREEN_WIDTH+0.0f) / (MAIN_SCREEN_HEIGHT+0.0f);



// Variables used by all 3D shader programs.
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 MVP;


// Screen has 0,0 in lower left while OpenGl has 0,0 in center and -1,-1 in lower left.
// This translates coordinates from screen to the OpenGl normated coordinates.
float translateScreenToNormatedY(int y)
{
	float f = 1.0 - (((float)y / (float)MAIN_SCREEN_HEIGHT) * 2.0);
	return f;
}

/*
static int translateNormatedToScreenY(float y)
{
	int i = (1.0-y) * (MAIN_SCREEN_HEIGHT/2);
	return i;
}
*/


// If there was a problem this is called to get some more understandable error message.
static void logProgramInfo(GLuint programId)
{
	if (glIsProgram(programId))
	{
		int maxLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);
		char* str = new char[maxLength]; // delete is done some lines down.
		int infoLogLength = 0;
		glGetProgramInfoLog(programId, maxLength, &infoLogLength, str);
		if (infoLogLength > 0)
		{
			printf("ProgramInfo: %s\n", str);
		}
		delete[] str;
	}
	else
	{
		printf("Invalid program name: %d\n", programId);
	}
}

// If there was a problem this is called to get some more understandable error message.
static void logShaderInfo(GLuint shaderId)
{
	if(glIsShader(shaderId))
	{
		int maxLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
		char* str = new char[maxLength]; // delete is done some lines down.
		int infoLogLength = 0;
		glGetShaderInfoLog(shaderId, maxLength, &infoLogLength, str);
		if (infoLogLength > 0)
		{
			printf("ShaderInfo: %s\n", str);
		}
		delete[] str;
	}
	else
	{
		printf( "Invalid shader name: %d\n", shaderId);
	}
}


// Check for error
int logGlError()
{
	const GLenum error = glGetError();
	if( error != GL_NO_ERROR )
	{
		#ifdef __EMSCRIPTEN__
			printf( "OpenGL error %d\n", (int)error);
		#else
			const GLubyte *str = gluErrorString(error);
			printf( "OpenGL error %d: %s\n", (int)error, (const char*)str );
		#endif
		return -1;
	}
	return 0;
}


GLint getAttribLocation(GLint cubeProgramId, const char* attribute_name)
{
	const GLint al = glGetAttribLocation(cubeProgramId, attribute_name);
	if (al == -1) {
		printf("Could not bind attribute %s\n", attribute_name);
		return -1;
	}
	return al;
}


GLint getUniformLocation(GLint cubeProgramId, const char* uniform_name)
{
	const GLint ul = glGetUniformLocation(cubeProgramId, uniform_name);
	if (ul == -1) {
	  fprintf(stderr, "Could not bind uniform '%s' in program %d\n", uniform_name, cubeProgramId);
	  return -1;
	}
	return ul;
}


GLuint createProgram(const GLchar* vertexShaderSource[], const GLchar* fragmentShaderSource[])
{
	// Generate program (empty here, will add shaders to it below)
	GLuint programId = glCreateProgram();

	// Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Shaders are written in a C-like language called GLSL. Read more at: http://www.raywenderlich.com/3664/opengl-tutorial-for-ios-opengl-es-2-0


	// Set vertex source
	glShaderSource( vertexShader, 1, vertexShaderSource, NULL );

	// Compile vertex source
	glCompileShader( vertexShader );


	// Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
	if( vShaderCompiled != GL_TRUE )
	{
		printf( "Failed to compile vertex shader %d!\n", vertexShader );
		logShaderInfo( vertexShader );
		return -1;
	}

	// Attach vertex shader to program
	glAttachShader( programId, vertexShader );




	// Create fragment shader
	GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );


	// Set fragment source
	glShaderSource( fragmentShader, 1, fragmentShaderSource, NULL );

	// Compile fragment source
	glCompileShader( fragmentShader );



	// Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
	if( fShaderCompiled != GL_TRUE )
	{
		printf( "Failed to compile fragment shader %d!\n", fragmentShader );
		logShaderInfo( fragmentShader );
		return -1;
	}


	// Attach fragment shader to program
	glAttachShader(programId, fragmentShader);



	// Link program
	glLinkProgram( programId );



	// Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(programId, GL_LINK_STATUS, &programSuccess);
	if( programSuccess != GL_TRUE )
	{
		printf( "Error linking program %d!\n", programId );
		logProgramInfo( programId );
		return -1;
	}


	return programId;
}


