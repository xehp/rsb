/*
 * ModelObjLoader.cpp
 *
 *  Created on: 29 Jan 2017
 *	  Author: alex
 */

#include "ModelObjLoader.h"
#include "Shaders.h"

#include "Pyramid3dProgram.h"


ModelObjLoader::ModelObjLoader(float x, float y, float z):
	vertexbuffer(0),
	uvbuffer(0),
	xPos(x),
	yPos(y),
	zPos(z)
{
}

ModelObjLoader::~ModelObjLoader()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
}

void ModelObjLoader::renderSelf()
{

	// For now, our models use a texture shader.
	glUseProgram(Shaders::shaderTexture);


	glm::mat4 modelMatrix = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			xPos, yPos, zPos, 1.0f
	);

	glm::mat4 savedMVP = MVP;


	// Calculate new translation/projection matrix
	MVP = MVP * modelMatrix; // Remember, matrix multiplication is the other way around

	// Tell GL to use the new translation matrix.
	glUniformMatrix4fv(pyramid3dMatrixID, 1, GL_FALSE, &MVP[0][0]);


	// The first attribute array is for verticies.
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // Attribute
		3,                  // Size
		GL_FLOAT,           // Type
		GL_FALSE,           // Normalized?
		0,                  // Stride
		(void*)0            // Array buffer offset
	);

	// The second attribute array is for UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // Attribute
		2,                                // Size
		GL_FLOAT,                         // Type
		GL_FALSE,                         // Normalized?
		0,                                // Stride
		(void*)0                          // Array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, out_vertices.size() );

	// Disable the first and second attribute array.
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	MVP = savedMVP;
}

bool ModelObjLoader::loadObjModel(const char* path)
{

	out_vertices.clear();
	out_uvs.clear();
	out_normals.clear();

	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> uvIndices;
	std::vector<unsigned int> normalIndices;

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* modelFile = fopen(path, "r");
	if(modelFile == NULL){
		printf("ERROR: Could not open model file!\n");
		return false;
	}

	// Read the file until it ends.
	while(1)
	{
		char lineHeader[128];

		// Read the first word of the line.
		int res = fscanf(modelFile, "%s", lineHeader);

		// Exit loop if the first word was not read.
		if (res == EOF)
			break;

		// Parse the line.
		if (strcmp( lineHeader, "v" ) == 0 )
		{
			glm::vec3 vertex;
			fscanf(modelFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);

		}
		else if ( strcmp( lineHeader, "vt" ) == 0 )
		{
			glm::vec2 uv;
			fscanf(modelFile, "%f %f\n", &uv.x, &uv.y );
			temp_uvs.push_back(uv);
		}
		else if ( strcmp( lineHeader, "vn" ) == 0 )
		{
			glm::vec3 normal;
			fscanf(modelFile, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}
		else if ( strcmp( lineHeader, "f" ) == 0 )
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(modelFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9)
			{
				printf("File can't be read by our simple parser :( Try exporting with other options\n");
				return false;
			}

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	// For each vertex of each triangle.
	for( unsigned int i=0; i<vertexIndices.size(); i++ )
	{


		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	}
	fclose(modelFile);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, out_vertices.size() * sizeof(glm::vec3), &out_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, out_uvs.size() * sizeof(glm::vec2), &out_uvs[0], GL_STATIC_DRAW);


	return true;
}
