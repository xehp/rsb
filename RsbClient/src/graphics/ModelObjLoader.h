/*
 * ModelObjLoader.h
 *
 *  Created on: 29 Jan 2017
 *      Author: alex
 */

#include "MirrorContainer.h"

#ifndef SRC_MODELOBJLOADER_H_
#define SRC_MODELOBJLOADER_H_

#include "includeGlEtc.h"
#include <vector>

class ModelObjLoader : public MirrorContainer
{
public:

	std::vector<glm::vec3> out_vertices;
	std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> out_normals;

	GLuint vertexbuffer;
	GLuint uvbuffer;

	ModelObjLoader(float x, float y, float z);
	virtual ~ModelObjLoader();
	bool loadObjModel(const char* path);
	virtual void renderSelf();

	float xPos;
	float yPos;
	float zPos;

};

#endif /* SRC_MODELOBJLOADER_H_ */
