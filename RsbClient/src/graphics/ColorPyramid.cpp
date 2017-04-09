/*
ColorPyramid.cpp

Copyright (C) 2015 Henrik Björkman www.eit.se/hb

History:
Created 2015 by Henrik Bjorkman www.eit.se





www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license



*/


// #include <SOIL/SOIL.h> // http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06


#include "includeGlEtc.h"
#include "CreateProgram.h"
#include "Pyramid3dProgram.h"


#ifndef __EMSCRIPTEN__
#include "hca.h"
#endif

#include "HybridLib.h"
#include "D3Vector.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"
#include "DrawText.h"
#include "ImageLoader.h"
#include "WordReader.h"
#include "MirrorContainer.h"
#include "ColorPyramid.h"


ColorPyramid::ColorPyramid(float x, float y, float z):
	MirrorContainer(),
	pyramidX(x),
	pyramidY(y),
	pyramidZ(z)
{
	// Some vertices for xyz axis
	{

		static const GLfloat vertex_buffer_data[] = {
			0.0f,  0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
			50.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,

			0.0f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
			0.0f, 50.0f, 0.0f,   0.0f, 1.0f, 0.0f,

			0.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 50.0f,   0.0f, 0.0f, 1.0f,
		};

		glGenBuffers(1, &pyramidAxisVertexBufferId); // Remember to do glDeleteBuffers for all generated buffers.
		glBindBuffer(GL_ARRAY_BUFFER, pyramidAxisVertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	}



	// Some vertices for a pyramid
	{
		static const GLfloat vertex_buffer_data[] = {
			0.0f,  1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f,     1.0f, 0.0f, 0.0f,

			0.0f,  1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,

			0.0f,  1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,

			0.0f,  1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f
		};

		glGenBuffers(1, &pyramidVertexBufferId); // Remember to do glDeleteBuffers for all generated buffers.
		glBindBuffer(GL_ARRAY_BUFFER, pyramidVertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	}




};

ColorPyramid::~ColorPyramid()
{
	glDeleteBuffers(1, &pyramidVertexBufferId);
	glDeleteBuffers(1, &pyramidAxisVertexBufferId);
};



void ColorPyramid::renderSelf()
{
	//MirrorContainer::renderAll();

		// Use our shader
		glUseProgram(pyramid3dProgramId);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);


		// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03

		glUniform1f(pyramid3dFadeId, 0.3f); // Set how transparent it is


		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(pyramid3dMatrixID, 1, GL_FALSE, &MVP[0][0]);


		// Tell GL to use the new translation matrix
		glUniformMatrix4fv(pyramid3dMatrixID, 1, GL_FALSE, &MVP[0][0]); // TODO is this line not a duplicate of the one above?

		glEnableVertexAttribArray(pyramid3d_coordinates_attribute);
		glEnableVertexAttribArray(pyramid3d_color_attribute);
		glBindBuffer(GL_ARRAY_BUFFER, pyramidAxisVertexBufferId);
		glVertexAttribPointer(
			pyramid3d_coordinates_attribute, // attribute.
			3,                     // size
			GL_FLOAT,              // type
			GL_FALSE,              // normalized?
			6 * sizeof(GLfloat),   // stride
			(void*)0               // array buffer offset
		);
		glVertexAttribPointer(
			pyramid3d_color_attribute,                // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                              // size
			GL_FLOAT,                       // type
			GL_FALSE,                       //  normalized?
			6 * sizeof(GLfloat),            // stride
			(GLvoid*) (3 * sizeof(GLfloat)) // array buffer offset
		);

		// draw the lines
		const int n = 6; // number of vertices, 6 for 3 lines.
		glDrawArrays(GL_LINES, 0, n);

		glDisableVertexAttribArray(pyramid3d_color_attribute);
		glDisableVertexAttribArray(pyramid3d_coordinates_attribute);



		// I want transparency, how is it enabled? Will look at his later, much later.

		// https://www.opengl.org/discussion_boards/showthread.php/175008-using-alpha-blending-with-shaders
		//glEnable( GL_DEPTH_TEST );
		//glEnable( GL_BLEND );
		//glEnable( GL_COLOR_MATERIAL );
		//glBlendEquation( GL_FUNC_ADD );
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


		// the pyramid
		{
			//glUniform1f(pyramid3dFadeId, 0.1); // not used (or not working)

			// Pyramid is not in Origo.
			glm::mat4 pyramidModel = glm::mat4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					pyramidX, pyramidY, pyramidZ, 1.0f
			);

			glm::mat4 savedMVP = MVP;

			// Calculate new translation/projection matrix
			MVP = MVP * pyramidModel; // Remember, matrix multiplication is the other way around

			// Tell GL to use the new translation matrix
			glUniformMatrix4fv(pyramid3dMatrixID, 1, GL_FALSE, &MVP[0][0]);


			glEnableVertexAttribArray(pyramid3d_coordinates_attribute);
			glEnableVertexAttribArray(pyramid3d_color_attribute);
			glBindBuffer(GL_ARRAY_BUFFER, pyramidVertexBufferId);
			glVertexAttribPointer(
				pyramid3d_coordinates_attribute, // attribute
				3,                     // size, the three room coordinates x,y,z.
				GL_FLOAT,              // type
				GL_FALSE,              // normalized?
				6 * sizeof(GLfloat),   // stride, the three room coordinates and the three colors
				(void*)0               // array buffer offset
			);
			glVertexAttribPointer(
				pyramid3d_color_attribute,        // attribute
				3,                              // size, the three colors r,g,b
				GL_FLOAT,                       // type
				GL_FALSE,                       //  normalized?
				6 * sizeof(GLfloat),            // stride, the three room coordinates and the three colors
				(GLvoid*) (3 * sizeof(GLfloat)) // array buffer offset, skipping the three room coordinates x,y,z
			);

			// draw the triangles
			const int n = 12; // number of vertices, 12 for 4 triangles.
			glDrawArrays(GL_TRIANGLES, 0, n);

			glDisableVertexAttribArray(pyramid3d_color_attribute);
			glDisableVertexAttribArray(pyramid3d_coordinates_attribute);

			MVP = savedMVP;
		}

	};
