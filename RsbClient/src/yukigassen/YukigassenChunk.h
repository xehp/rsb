/*
 * YukigassenChunk.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Oct 15, 2015
 *      Author: drekkar
 */

#ifndef YUKIGASSENCHUNK_H_
#define YUKIGASSENCHUNK_H_

#include <vector>
#include "includeGlEtc.h"
#include "HashMap.h"
#include "D3Int.h"
#include "YukigassenVoxelMap.h"


class YukigassenChunk {
public:
	YukigassenChunk(long long chunkIndex);
	virtual ~YukigassenChunk();

	enum
	{
	  // The numbers here needs to be same as in YukigassenVoxelRoom.java
	  BitsUsedForBlockMode = 2,
	  BitsPerColor = 2,
	  BitsUsedForColorOrTexture = 6,
	  PositionForBlockMode = 6, // TODO change to 6
      PositionForColor = 0,  // TODO change to 0
      ColorOrTextureMask = (1<<BitsUsedForColorOrTexture)-1,
      NumberOfColorsPerColor = 1 << BitsPerColor,
      BitsPerColorMask= (1<<BitsPerColor)-1,
      BlockModeMask= (1<<BitsUsedForBlockMode)-1,
	};

	enum
	{
	  invisibleBlock = 0,
	  nonOpaqueTextureBlock = 1,
	  coloredOpaqueBlock = 2,
	  opaqueTextureBlock = 3
	};

	enum
	{
		EmptyBlock=0
	};

	// Call reset before adding lines and triangles.
	void reset();

	void addToLineVertexBuffer(float x, float y, float z, float r, float g, float b);
	void addToTriangleVertexBuffer(float x, float y, float z, float r, float g, float b);
	void addLineBox(float x, float y, float z, float sx2, float sy2, float sz2);
	void addTriangleBox(float x, float y, float z, float sx2, float sy2, float sz2, int neighborMask, int color);
	void addToTexVertexBuffer(float x, float y, float z);
	void addToTexCoordsBuffer(float x, float y);
	void addToTexElementsBuffer(int a, int b, int c);

	// Call setup when all lines and triangles has been added.
	void setup();

	void renderSelf();
	void addYukigassenVoxelMap(YukigassenVoxelMap *yvm);
	//void setupInternalRenderingDataPartial(long long voxelMapIndex);

	// Mode and color are packed into one byte. If packColorAndMode in java is changed some change here will also be needed.
	// The bit shifting here needs to match that used in method packColorAndMode in YukigassenVoxelRoom.java
	static int getBlockRed(int colorAndMode) {return (colorAndMode>>(PositionForColor+(2*BitsPerColor)))&(BitsPerColorMask);};
	static int getBlockGreen(int colorAndMode) {return (colorAndMode>>(PositionForColor+BitsPerColor))&(BitsPerColorMask);};
	static int getBlockBlue(int colorAndMode) {return (colorAndMode>>PositionForColor)&(BitsPerColorMask);};
	static int getBlockMode(int colorAndMode) {return (colorAndMode >> PositionForBlockMode) & (BlockModeMask);};
	static bool getBlockInvisible(int colorAndMode) {return getBlockMode(colorAndMode)==invisibleBlock;};
	static int getBlockTextureIndex(int colorAndMode) {return (colorAndMode>>PositionForColor)&ColorOrTextureMask;};


	GLuint linesVertexBufferId;
	int linesNVertexesToRender;
	size_t linesVertexBufferSize; // size is in number of vertices it can store (not bytes), each line is FLOATS_PER_COLOR_VERTEX*COLOR_VERTICES_PER_LINE float
	std::vector<float> linesVertexBufferVector; // Here we store pairs of floats that make up lines (AKA filaments) that we will then move into GL buffer by calling glBufferData.


	GLuint triangleVertexBufferId;
	int triangleNVertexesToRender;
	size_t triangleVertexBufferSize; // size is in number of vertices it can store (not bytes), each line is FLOATS_PER_COLOR_VERTEX*COLOR_VERTICES_PER_LINE float
	std::vector<float> triangleVertexBufferVector; // Same as linesVertexBufferVector but for triangles.


	GLuint tex_vbo_vertices;
	GLuint tex_ibo_elements;
	GLuint tex_vbo_texcoords;
	//static GLuint tex_texture_id;
	int tex_cube_vertices_to_render;
	size_t tex_cube_vertices_vector_size;
	std::vector<float> tex_cube_vertices_vector;

	int tex_cube_texcoordsToRender;
	size_t tex_cube_texcoordsVectorSize;
	std::vector<float> tex_cube_texcoordsVector;

	int tex_cube_elementsToRender;
	size_t tex_cube_elementsVectorSize;
	std::vector<GLushort> tex_cube_elementsVector;

	int tex_boxCount;


	D3Int chunkSizeInBlocks;
	D3Int chunkSizeInVoxelMaps;

	HashMap_64_32 yukigassenVoxelMapList;

	bool needUpdate;
	long long chunkIndex;
};

#endif /* YUKIGASSENCHUNK_H_ */
