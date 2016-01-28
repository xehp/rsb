/*
 * CreateProgram.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 */

#ifndef CREATEPROGRAM_H_
#define CREATEPROGRAM_H_

#define USE_OPENGL_2_1

extern const int MAIN_SCREEN_WIDTH;
extern const int MAIN_SCREEN_HEIGHT;
extern const float mainAspectRatio;

extern glm::mat4 Projection;
extern glm::mat4 View;
extern glm::mat4 MVP;

float translateScreenToNormatedY(int y);


int logGlError();

GLint getUniformLocation(GLint cubeProgramId, const char* uniform_name);
GLint getAttribLocation(GLint cubeProgramId, const char* attribute_name);
GLuint createProgram(const GLchar* vertexShaderSource[], const GLchar* fragmentShaderSource[]);




#endif /* CREATEPROGRAM_H_ */
