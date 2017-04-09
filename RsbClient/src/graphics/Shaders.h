/*
 * Shader.h
 *
 *  Created on: 31 Jan 2017
 *      Author: alex
 */

#ifndef SRC_SHADERS_H_2
#define SRC_SHADERS_H_2

#include "includeGlEtc.h"

class Shaders {

public:

	/** A default shader for textures. */
	static GLuint shaderTexture;

	/** Load all default shaders. */
	static int loadDefaultShaders();

	static GLuint loadShader(const char * vertex_file_path,const char * fragment_file_path);
};



#endif /* SRC_SHADERS_H_ */
