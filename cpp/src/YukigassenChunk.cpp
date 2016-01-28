/*
 * YukigassenChunk.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: Oct 15, 2015
 *      Author: drekkar
 */

#include "includeGlEtc.h"
#include "ImageLoader.h"
#include "Cube3dProgram.h"
#include "Pyramid3dProgram.h"
#include "YukigassenVoxelMap.h"
#include "YukigassenVoxelRoom.h"
#include "YukigassenChunk.h"


/*

OpenGL on screen:
x is in the plane of the screen, positive is left.
y is in the plane of the screen, positive is up on the screen.
z is perpendicular to the screen, positive us up above/closer to viewer.
World is initially seen from above looking down.
And forward is up on the screen.


In a model on the other hand:
x is forward
y is right
z is down



Box corners are named: a, b, c, d, e, f, g, h


           y
           ^
           |
           |
           |
        f---------e
       /         /|
      / |       / |
     /         /  |
    b---------a   |   ------> x
    |   g  -  | - h
    |         |  /
    | /       | /
    |         |/
    c---------d
      /
     /
    /
   z


in XYZ r
a = +r, +r, +r
b = -r, +r, +r
c = -r, -r, +r
d = +r, -r, +r
e = +r, +r, -r
f = -r, +r, -r
g = -r, -r, -r
h = +r, -r, -r

Surfaces are named after axis and positive or negative along that axis
corners    Surface
abcd 0123    zp    bottom (the surface seen if looking from positive Z axis)
adhe 0345    xp    front
aefb 0561    yp    right
bfgc 1672    xn    rear
cghd 7432    yn    left
efgh 4567    zn    top



Surfaces are also numbered.
xp = s0 (front)
yp = s1 (right)
zp = s2 (bottom)
zn = s3 (top)
yn = s4 (left)
xn = s5 (rear)




Or as used at http://www.songho.ca/opengl/gl_vertexarray.html


           y
           ^
           |
           |
           |
       v6--------v5
       /         /|
      / |       / |
     /         /  |
   v1--------v0   |   ------> x
    |  v7  -  |  v4
    |         |  /
    | /       | /
    |         |/
   v2--------v3
      /
     /
    /
   z


Translating between the two:
a=v0, b=v1, c=v2, d=v3, e=v5, f=v6, g=v7, h=v4





*/
//GLuint YukigassenChunk::tex_texture_id=ZERO_GL;



YukigassenChunk::YukigassenChunk(long long chunkIndex):
	linesVertexBufferId(ZERO_GL),
	linesNVertexesToRender(0),
	linesVertexBufferSize(0),
	linesVertexBufferVector(),
	triangleVertexBufferId(ZERO_GL),
	triangleNVertexesToRender(0),
	triangleVertexBufferSize(0),
	triangleVertexBufferVector(),
	tex_vbo_vertices(ZERO_GL),
	tex_ibo_elements(ZERO_GL),
	tex_vbo_texcoords(ZERO_GL),
	//tex_texture_id(ZERO_GL),
	tex_cube_vertices_to_render(0),
	tex_cube_vertices_vector_size(0),
	tex_cube_vertices_vector(),
	tex_cube_texcoordsToRender(0),
	tex_cube_texcoordsVectorSize(0),
	tex_cube_texcoordsVector(),
	tex_cube_elementsToRender(0),
	tex_cube_elementsVectorSize(0),
	tex_cube_elementsVector(),
	tex_boxCount(0),
	yukigassenVoxelMapList(),
	needUpdate(false),
	chunkIndex(chunkIndex)
{
	linesVertexBufferSize = 8;
	linesVertexBufferVector.resize(FLOATS_PER_COLOR_VERTEX*linesVertexBufferSize);

	triangleVertexBufferSize = 64;
	triangleVertexBufferVector.resize(FLOATS_PER_COLOR_VERTEX*triangleVertexBufferSize);

	tex_cube_vertices_vector_size=32;
	tex_cube_vertices_vector.resize(FLOATS_PER_TEX_VERTEX*tex_cube_vertices_vector_size);

	tex_cube_texcoordsVectorSize=32;
	tex_cube_texcoordsVector.resize(FLOATS_PER_TEX_COORD*tex_cube_texcoordsVectorSize);

	tex_cube_elementsVectorSize=16;
	tex_cube_elementsVector.resize(SHORTS_PER_ELEMENT*tex_cube_elementsVectorSize);

}

YukigassenChunk::~YukigassenChunk() {
	// Make sure to free old buffer if any.
	glDeleteBuffers(1, &linesVertexBufferId);
	glDeleteBuffers(1, &triangleVertexBufferId);

	glDeleteBuffers(1, &tex_vbo_vertices);
}


void YukigassenChunk::reset()
{
	linesNVertexesToRender = 0;
	triangleNVertexesToRender = 0;
	tex_cube_vertices_to_render = 0;
	tex_cube_texcoordsToRender = 0;
	tex_cube_elementsToRender = 0;

	tex_boxCount=0;
};


void YukigassenChunk::setup()
{
	//glUseProgram(pyramid3dProgramId); // This line is probably not needed here.


	glDeleteBuffers(1, &linesVertexBufferId); linesVertexBufferId = ZERO_GL; // Make sure to free old buffer if any.
	glGenBuffers(1, &linesVertexBufferId); // Remember to do glDeleteBuffers for all generated buffers.
	glBindBuffer(GL_ARRAY_BUFFER, linesVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*FLOATS_PER_COLOR_VERTEX*linesNVertexesToRender, linesVertexBufferVector.data(), GL_STATIC_DRAW);


	glDeleteBuffers(1, &triangleVertexBufferId); triangleVertexBufferId = ZERO_GL; // Make sure to free old buffer if any.
	glGenBuffers(1, &triangleVertexBufferId); // Remember to do glDeleteBuffers for all generated buffers.
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*FLOATS_PER_COLOR_VERTEX*triangleNVertexesToRender, triangleVertexBufferVector.data(), GL_STATIC_DRAW);


	// This is the corners of the cube or cuboid to be rendered.
	glDeleteBuffers(1, &tex_vbo_vertices); tex_vbo_vertices = ZERO_GL; // Make sure to free old buffer if any.
	glGenBuffers(1, &tex_vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*FLOATS_PER_TEX_VERTEX*tex_cube_vertices_to_render, tex_cube_vertices_vector.data(), GL_STATIC_DRAW);

	// This will tell GL which part of the texture to use for each surface rendered.
	glDeleteBuffers(1, &tex_vbo_texcoords); tex_vbo_texcoords = ZERO_GL; // Make sure to free old buffer if any.
	glGenBuffers(1, &tex_vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*FLOATS_PER_TEX_COORD*tex_cube_texcoordsToRender, tex_cube_texcoordsVector.data(), GL_STATIC_DRAW);

	// This is used to tell in which order we use the cube_vertices, this makes it possible to use vertices more than once.
	glDeleteBuffers(1, &tex_ibo_elements); tex_ibo_elements = ZERO_GL; // Make sure to free old buffer if any.
	glGenBuffers(1, &tex_ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tex_ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*SHORTS_PER_ELEMENT*tex_cube_elementsToRender, tex_cube_elementsVector.data(), GL_STATIC_DRAW);

}


void YukigassenChunk::addToTexVertexBuffer(float x, float y, float z)
{
	if(tex_cube_vertices_to_render>=tex_cube_vertices_vector_size)
	{
		tex_cube_vertices_vector_size *=2;
		tex_cube_vertices_vector.resize(tex_cube_vertices_vector_size*FLOATS_PER_TEX_VERTEX);
	}
	float* ptr=tex_cube_vertices_vector.data() + (tex_cube_vertices_to_render * FLOATS_PER_TEX_VERTEX);
	*ptr++ = x;
	*ptr++ = y;
	*ptr++ = z;
	++tex_cube_vertices_to_render;
}

void YukigassenChunk::addToTexCoordsBuffer(float x, float y)
{
	if(tex_cube_texcoordsToRender>=tex_cube_texcoordsVectorSize)
	{
		tex_cube_texcoordsVectorSize *=2;
		tex_cube_texcoordsVector.resize(tex_cube_texcoordsVectorSize*FLOATS_PER_TEX_COORD);
	}
	float* ptr=tex_cube_texcoordsVector.data() + (tex_cube_texcoordsToRender * FLOATS_PER_TEX_COORD);
	*ptr++ = x;
	*ptr++ = y;
	++tex_cube_texcoordsToRender;
}

void YukigassenChunk::addToTexElementsBuffer(int a, int b, int c)
{
	if(tex_cube_elementsToRender>=tex_cube_elementsVectorSize)
	{
		tex_cube_elementsVectorSize *=2;
		tex_cube_elementsVector.resize(tex_cube_elementsVectorSize*SHORTS_PER_ELEMENT);
	}
	GLushort* ptr=tex_cube_elementsVector.data() + (tex_cube_elementsToRender * SHORTS_PER_ELEMENT);
	*ptr++ = tex_boxCount+a;
	*ptr++ = tex_boxCount+b;
	*ptr++ = tex_boxCount+c;
	++tex_cube_elementsToRender;
}


void YukigassenChunk::addToLineVertexBuffer(float x, float y, float z, float r, float g, float b)
{
	if(linesNVertexesToRender>=linesVertexBufferSize)
	{
		linesVertexBufferSize *=2;
		linesVertexBufferVector.resize(linesVertexBufferSize*FLOATS_PER_COLOR_VERTEX);
	}
	//GLfloat* ptr=linesVertexBufferData + (linesNVertexesToRender * FLOATS_PER_COLOR_VERTEX);
	float* ptr=linesVertexBufferVector.data() + (linesNVertexesToRender * FLOATS_PER_COLOR_VERTEX);
	*ptr++ = x;
	*ptr++ = y;
	*ptr++ = z;
	*ptr++ = r;
	*ptr++ = g;
	*ptr++ = b;
	++linesNVertexesToRender;
}

// Add one vertex to the triangle buffer, each triangle will need 3 vertices.
void YukigassenChunk::addToTriangleVertexBuffer(float x, float y, float z, float r, float g, float b)
{
	if(triangleNVertexesToRender>=triangleVertexBufferSize)
	{
		triangleVertexBufferSize *=2;
		triangleVertexBufferVector.resize(triangleVertexBufferSize*FLOATS_PER_COLOR_VERTEX);
	}

	float* ptr=triangleVertexBufferVector.data() + (triangleNVertexesToRender * FLOATS_PER_COLOR_VERTEX);
	*ptr++ = x;
	*ptr++ = y;
	*ptr++ = z;
	*ptr++ = r;
	*ptr++ = g;
	*ptr++ = b;
	++triangleNVertexesToRender;
}


// Create and add to rendering a filament box.
void YukigassenChunk::addLineBox(float x, float y, float z, float sx2, float sy2, float sz2)
{
	addToLineVertexBuffer(x+sx2, y+sy2, z+sz2,  1.0f, 1.0f, 1.0f);
	addToLineVertexBuffer(x+sx2, y+sy2, z-sz2,  1.0f, 1.0f, 0.0f);

	addToLineVertexBuffer(x+sx2, y+sy2, z+sz2,  1.0f, 1.0f, 1.0f);
	addToLineVertexBuffer(x+sx2, y-sy2, z+sz2,  1.0f, 0.0f, 1.0f);

	addToLineVertexBuffer(x+sx2, y-sy2, z+sz2,  1.0f, 0.0f, 1.0f);
	addToLineVertexBuffer(x+sx2, y-sy2, z-sz2,  1.0f, 0.0f, 0.0f);

	addToLineVertexBuffer(x+sx2, y+sy2, z-sz2,  1.0f, 1.0f, 0.0f);
	addToLineVertexBuffer(x+sx2, y-sy2, z-sz2,  1.0f, 0.0f, 0.0f);


	addToLineVertexBuffer(x-sx2, y+sy2, z+sz2,  0.0f, 1.0f, 1.0f);
	addToLineVertexBuffer(x-sx2, y+sy2, z-sz2,  0.0f, 1.0f, 0.0f);

	addToLineVertexBuffer(x-sx2, y+sy2, z+sz2,  0.0f, 1.0f, 1.0f);
	addToLineVertexBuffer(x-sx2, y-sy2, z+sz2,  0.0f, 0.0f, 1.0f);

	addToLineVertexBuffer(x-sx2, y-sy2, z+sz2,  0.0f, 0.0f, 1.0f);
	addToLineVertexBuffer(x-sx2, y-sy2, z-sz2,  0.0f, 0.0f, 0.0f);

	addToLineVertexBuffer(x-sx2, y+sy2, z-sz2,  0.0f, 1.0f, 0.0f);
	addToLineVertexBuffer(x-sx2, y-sy2, z-sz2,  0.0f, 0.0f, 0.0f);


	addToLineVertexBuffer(x-sx2, y+sy2, z+sz2,  0.0f, 1.0f, 1.0f);
	addToLineVertexBuffer(x+sx2, y+sy2, z+sz2,  1.0f, 1.0f, 1.0f);

	addToLineVertexBuffer(x-sx2, y+sy2, z-sz2,  0.0f, 1.0f, 0.0f);
	addToLineVertexBuffer(x+sx2, y+sy2, z-sz2,  1.0f, 1.0f, 0.0f);

	addToLineVertexBuffer(x-sx2, y-sy2, z+sz2,  0.0f, 0.0f, 1.0f);
	addToLineVertexBuffer(x+sx2, y-sy2, z+sz2,  1.0f, 0.0f, 1.0f);

	addToLineVertexBuffer(x-sx2, y-sy2, z-sz2,  0.0f, 0.0f, 0.0f);
	addToLineVertexBuffer(x+sx2, y-sy2, z-sz2,  1.0f, 0.0f, 0.0f);

}


// Create a box using 12 triangles
// neighborMask tells if the surface is visible. If this and the neighbor are opaque the the surface is not visible and so it don't need rendering.
void YukigassenChunk::addTriangleBox(float x, float y, float z, float sx2, float sy2, float sz2, int neighborMask, int colorAndMode)
{
	// corner a alias v0
    const float ax=x+sx2;
    const float ay=y+sy2;
    const float az=z+sz2;

	// corner b alias v1
    const float bx=x-sx2;
    const float by=y+sy2;
    const float bz=z+sz2;

	// corner c alias v2
    const float cx=x-sx2;
    const float cy=y-sy2;
    const float cz=z+sz2;

    // corner d alias v3
    const float dx=x+sx2;
    const float dy=y-sy2;
    const float dz=z+sz2;

    // corner e alias v5
    const float ex=x+sx2;
    const float ey=y+sy2;
    const float ez=z-sz2;

    // corner f alias v6
    const float fx=x-sx2;
    const float fy=y+sy2;
    const float fz=z-sz2;

    // corner g alias v7
    const float gx=x-sx2;
    const float gy=y-sy2;
    const float gz=z-sz2;

    // corner h alias v4
    const float hx=x+sx2;
    const float hy=y-sy2;
    const float hz=z-sz2;

    const int mode = getBlockMode(colorAndMode);

    if (mode == coloredOpaqueBlock)
    {
    	// A plain colored block

		const int red = getBlockRed(colorAndMode);
		const int green = getBlockGreen(colorAndMode);
		const int blue = getBlockBlue(colorAndMode);
		//const int alpha = b&0x3;

		const float r = (float)red/(float)(NumberOfColorsPerColor-1);
		const float g = (float)green/(float)(NumberOfColorsPerColor-1);
		const float b = (float)blue/(float)(NumberOfColorsPerColor-1);


		// bottom: zp = abcd = abc & acd
		if ((neighborMask & YukigassenVoxelRoom::MaskZn) == 0)
		{
			const float light=0.5f;  // Our shader don't simulate lightning yet so fake it here.
			const float lr = light * r;
			const float lg = light * g;
			const float lb = light * b;
			addToTriangleVertexBuffer(ax, ay, az,  lr, lg, lb*0.9f);
			addToTriangleVertexBuffer(bx, by, bz,  lr, lg, lb);
			addToTriangleVertexBuffer(cx, cy, cz,  lr, lg, lb);

			// For debugging I sometimes comment out these 3 lines.
			addToTriangleVertexBuffer(ax, ay, az,  lr, lg, lb*0.9f);
			addToTriangleVertexBuffer(cx, cy, cz,  lr, lg, lb);
			addToTriangleVertexBuffer(dx, dy, dz,  lr, lg, lb);
		}

		// front: xp = adhe = adh & ahe
		if ((neighborMask & YukigassenVoxelRoom::MaskXn) == 0)
		{
			const float light=0.6f;  // Our shader don't simulate lightning yet so fake it here.
			const float lr = light * r;
			const float lg = light * g;
			const float lb = light * b;
			addToTriangleVertexBuffer(ax, ay, az,  lr*0.9, lg, lb);
			addToTriangleVertexBuffer(dx, dy, dz,  lr, lg, lb);
			addToTriangleVertexBuffer(hx, hy, hz,  lr, lg, lb);

			addToTriangleVertexBuffer(ax, ay, az,  lr*0.9, lg, lb);
			addToTriangleVertexBuffer(hx, hy, hz,  lr, lg, lb);
			addToTriangleVertexBuffer(ex, ey, ez,  lr, lg, lb);
		}

		// right: yp = aefb = aef & afb
		if ((neighborMask & YukigassenVoxelRoom::MaskYn) == 0)
		{
			const float light=0.7f;  // Our shader don't simulate lightning yet so fake it here.
			const float lr = light * r;
			const float lg = light * g;
			const float lb = light * b;
			addToTriangleVertexBuffer(ax, ay, az,  lr, lg*0.9f, lb);
			addToTriangleVertexBuffer(ex, ey, ez,  lr, lg, lb);
			addToTriangleVertexBuffer(fx, fy, fz,  lr, lg, lb);

			addToTriangleVertexBuffer(ax, ay, az,  lr, lg*0.9f, lb);
			addToTriangleVertexBuffer(fx, fy, fz,  lr, lg, lb);
			addToTriangleVertexBuffer(bx, by, bz,  lr, lg, lb);
		}

		// back xn = bfgc = bfg & bgc
		if ((neighborMask & YukigassenVoxelRoom::MaskXp) == 0)
		{
			const float light=0.8f;  // Our shader don't simulate lightning yet so fake it here.
			const float lr = light * r;
			const float lg = light * g;
			const float lb = light * b;
			addToTriangleVertexBuffer(bx, by, bz,  lr*0.9f, lg, lb);
			addToTriangleVertexBuffer(fx, fy, fz,  lr, lg, lb);
			addToTriangleVertexBuffer(gx, gy, gz,  lr, lg, lb);

			addToTriangleVertexBuffer(bx, by, bz,  lr*0.9f, lg, lb);
			addToTriangleVertexBuffer(gx, gy, gz,  lr, lg, lb);
			addToTriangleVertexBuffer(cx, cy, cz,  lr, lg, lb);
		}

		// left: yn = cghd = cgh & chd
		if ((neighborMask & YukigassenVoxelRoom::MaskYp) == 0)
		{
			const float light=0.9f;  // Our shader don't simulate lightning yet so fake it here.
			const float lr = light * r;
			const float lg = light * g;
			const float lb = light * b;

			addToTriangleVertexBuffer(cx, cy, cz,  lr, lg*0.9, lb);
			addToTriangleVertexBuffer(gx, gy, gz,  lr, lg, lb);
			addToTriangleVertexBuffer(hx, hy, hz,  lr, lg, lb);

			addToTriangleVertexBuffer(cx, cy, cz,  lr, lg*0.9, lb);
			addToTriangleVertexBuffer(hx, hy, hz,  lr, lg, lb);
			addToTriangleVertexBuffer(dx, dy, dz,  lr, lg, lb);
		}

		// top: zn = efgh = efg & egh
		if ((neighborMask & YukigassenVoxelRoom::MaskZp) == 0)
		{
			const float light=1.0f;  // Our shader don't simulate lightning yet so fake it here.
			const float lr = light * r;
			const float lg = light * g;
			const float lb = light * b;

			addToTriangleVertexBuffer(ex, ey, ez,  lr, lg, lb*0.9f);
			addToTriangleVertexBuffer(gx, gy, gz,  lr, lg, lb);
			addToTriangleVertexBuffer(fx, fy, fz,  lr, lg, lb);

			addToTriangleVertexBuffer(ex, ey, ez,  lr, lg, lb*0.9f);
			addToTriangleVertexBuffer(hx, hy, hz,  lr, lg, lb);
			addToTriangleVertexBuffer(gx, gy, gz,  lr, lg, lb);
		}
    }
    else
    {
    	// A textured block

    	int textureIndex = getBlockTextureIndex(colorAndMode);



    	//if (tex_boxCount==0)
    	{
    		const float nTexturesX = cube3dtextureWidth/32.0f; // How many textures in X
    		const float nTexturesY = cube3dtextureHeight/32.0f; // How many textures in Y

    		const float yOffset=textureIndex/nTexturesY;
			const float ySize=1.0f/nTexturesY;
			const float yEnd=yOffset+ySize;



			// a=v0, b=v1, c=v2, d=v3, e=v5, f=v6, g=v7, h=v4

			// front alias Xp, s0, surface 1.
			//if ((neighborMask & YukigassenVoxelRoom::MaskXn) == 0)    // TODO: Can not use this optimization yet since element numbering must be adjusted if used.
			{
				addToTexVertexBuffer(ax,  ay,  az);   //  0 v0 a
				addToTexVertexBuffer(dx,  dy,  dz);   //  1 v3 d
				addToTexVertexBuffer(hx,  hy,  hz);   //  2 v4 h
				addToTexVertexBuffer(ex,  ey,  ez);   //  3 v5 e

				addToTexCoordsBuffer(0.0,           yEnd);
				addToTexCoordsBuffer(1.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(1.0/nTexturesX, yOffset);
				addToTexCoordsBuffer(0.0,           yOffset);

				addToTexElementsBuffer(0,  1,  2);   // v0 v3 v4
				addToTexElementsBuffer(2,  3,  0);   // v4 v5 v0
			}

			// right, alias Yp, s1, surface 2.
			//if ((neighborMask & YukigassenVoxelRoom::MaskYn) == 0)
			{
				addToTexVertexBuffer(bx,  by,  bz);   //  4 v1 b
				addToTexVertexBuffer(ax,  ay,  az);   //  5 v0 a
				addToTexVertexBuffer(ex,  ey,  ez);   //  6 v5 e
				addToTexVertexBuffer(fx,  fy,  fz);   //  7 v6 f

				addToTexCoordsBuffer(1.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(2.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(2.0/nTexturesX, yOffset);
				addToTexCoordsBuffer(1.0/nTexturesX, yOffset);

				addToTexElementsBuffer(4,  5,  6);   // v1 v0 v5
				addToTexElementsBuffer(6,  7,  4);   // v5 v6 v1
			}

			// bottom, alias Zp, s2, surface 3
			//if ((neighborMask & YukigassenVoxelRoom::MaskZn) == 0)
			{
				addToTexVertexBuffer(bx,  by,  bz);   //  8 v1 b
				addToTexVertexBuffer(cx,  cy,  cz);  //  9 v2 c
				addToTexVertexBuffer(dx,  dy,  dz);  // 10 v3 d
				addToTexVertexBuffer(ax,  ay,  az);  // 11 v0 a

				addToTexCoordsBuffer(2.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(3.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(3.0/nTexturesX, yOffset);
				addToTexCoordsBuffer(2.0/nTexturesX, yOffset);

				addToTexElementsBuffer(8,  9, 10);   // v1 v2 v3
				addToTexElementsBuffer(10, 11,  8);   // v3 v0 v1
			}

			// top, alias Zn, s3, surface 4
			//if ((neighborMask & YukigassenVoxelRoom::MaskZp) == 0)
			{
				addToTexVertexBuffer(ex, ey,  ez); // 12 v5 e
				addToTexVertexBuffer(hx, hy,  hz);  // 13 v4 h
				addToTexVertexBuffer(gx, gy, gz); // 14 v7 g
				addToTexVertexBuffer(fx, fy, fz);  // 15 v6 f

				addToTexCoordsBuffer(3.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(4.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(4.0/nTexturesX, yOffset);
				addToTexCoordsBuffer(3.0/nTexturesX, yOffset);

				addToTexElementsBuffer(12, 13, 14);   // v5 v4 v7
				addToTexElementsBuffer(14, 15, 12);   // v7 v6 v5
			}

			// left, alias Yn, s4, surface 5
			//if ((neighborMask & YukigassenVoxelRoom::MaskYp) == 0)
			{
				addToTexVertexBuffer(dx, dy, dz);  // 16 v3 d
				addToTexVertexBuffer(cx, cy, cz);  // 17 v2 c
				addToTexVertexBuffer(gx, gy, gz);  // 18 v7 g
				addToTexVertexBuffer(hx, hy, hz);  // 19 v4 h

				addToTexCoordsBuffer(4.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(5.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(5.0/nTexturesX, yOffset);
				addToTexCoordsBuffer(4.0/nTexturesX, yOffset);

				addToTexElementsBuffer(16, 17, 18);   // v3 v2 v7
				addToTexElementsBuffer(18, 19, 16);   // v7 v4 v3
			}

			// back, alias Xn, s5, surface 6
			//if ((neighborMask & YukigassenVoxelRoom::MaskXp) == 0)
			{
				addToTexVertexBuffer(cx, cy, cz);  // 20 v2 c
				addToTexVertexBuffer(bx, by, bz);  // 21 v1 b
				addToTexVertexBuffer(fx, fy, fz);  // 22 v6 f
				addToTexVertexBuffer(gx, gy, gz);  // 23 v7 g

				addToTexCoordsBuffer(5.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(6.0/nTexturesX, yEnd);
				addToTexCoordsBuffer(6.0/nTexturesX, yOffset);
				addToTexCoordsBuffer(5.0/nTexturesX, yOffset);

				addToTexElementsBuffer(20, 21, 22);   // v2 v1 v6
				addToTexElementsBuffer(22, 23, 20);   // v6 v7 v2
			}



			tex_boxCount+=24;
    	} // if (texCount==0)
    } // if (mode == coloredBlock)
}


void YukigassenChunk::renderSelf()
{
	// Read more at: https://gitlab.com/wikibooks-opengl/modern-tutorials/blob/master/tut06_textures/cube.cpp

	glEnable(GL_CULL_FACE); // TODO it should be enabled already when we get here.

	//D3ObjWithPos::renderSelf();


	glUseProgram(pyramid3dProgramId);
	glDisable(GL_BLEND);


	// Tell GL to use the new translation matrix
	glUniformMatrix4fv(pyramid3dMatrixID, 1, GL_FALSE, &MVP[0][0]);


	// This draws a line around the room (used for debugging)
	glEnableVertexAttribArray(pyramid3d_coordinates_attribute);
	glEnableVertexAttribArray(pyramid3d_color_attribute);
	glBindBuffer(GL_ARRAY_BUFFER, linesVertexBufferId);
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
	glDrawArrays(GL_LINES, 0, linesNVertexesToRender);

	glDisableVertexAttribArray(pyramid3d_color_attribute);
	glDisableVertexAttribArray(pyramid3d_coordinates_attribute);




	// This draws all the colored block surfaces in the room
	glEnableVertexAttribArray(pyramid3d_coordinates_attribute);
	glEnableVertexAttribArray(pyramid3d_color_attribute);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBufferId);
	glVertexAttribPointer(
		pyramid3d_coordinates_attribute, // attribute.
		3,                     // size
		GL_FLOAT,              // type
		GL_FALSE,              // normalized?
		6 * sizeof(GLfloat),   // stride
		(void*)0               // array buffer offset
	);
	glVertexAttribPointer(
		pyramid3d_color_attribute,        // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                              // size
		GL_FLOAT,                       // type
		GL_FALSE,                       //  normalized?
		6 * sizeof(GLfloat),            // stride
		(GLvoid*) (3 * sizeof(GLfloat)) // array buffer offset
	);

	// draw the lines
	glDrawArrays(GL_TRIANGLES, 0, triangleNVertexesToRender);

	glDisableVertexAttribArray(pyramid3d_color_attribute);
	glDisableVertexAttribArray(pyramid3d_coordinates_attribute);





/*
	// Now render selected voxel if any for now choose index pos in upper corner
	D3Int gridXyz(0,0,roomSizeInBlocks.z-1);

	D3Vector pos(gridXyz.x, gridXyz.y, gridXyz.z);


	// The MVP is already set for coordinates of this object. So to get a position some distance in front of avatar just add the distance to X.
	glm::mat4 handModel = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			//pao.nose.x*4.0f, pao.nose.y*4.0f, pao.nose.z*4.0f, 1.0f
			4.0f, 0.0f, 0.0f, 1.0f
	);

	// Calculate new translation/projection matrix
	MVP = MVP * handModel; // Remember, matrix multiplication is the other way around

*/



	// rendering of textured surfaces

	if (tex_boxCount!=0)
	{
		glUseProgram(cube3dProgramId);


		// Tell GL to use the new translation matrix
		glUniformMatrix4fv(cube3dMatrixID, 1, GL_FALSE, &MVP[0][0]);

		//glEnable(GL_CULL_FACE);
		glDisable(GL_CULL_FACE); // TODO disabled just for testing...

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cube3dTextureId);
		glUniform1i(cube3dUniformMytexture, /*GL_TEXTURE*/0);


		glEnableVertexAttribArray(cube3dCoord3dAttribute);
		glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_vertices);
		glVertexAttribPointer(
			cube3dCoord3dAttribute, // attribute
			3,                     // size, the three room coordinates x,y,z.
			GL_FLOAT,              // type
			GL_FALSE,              // normalized? No, take our values as-is.
			3 * sizeof(GLfloat),   // stride
			(void*)0               // array buffer offset, offset of first element
		);

		glEnableVertexAttribArray(cube3dTexcoord_attribute);
		glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_texcoords);
		glVertexAttribPointer(
			cube3dTexcoord_attribute, // attribute
		    2,                  // number of elements per vertex, here (x,y)
		    GL_FLOAT,           // the type of each element
		    GL_FALSE,           // take our values as-is
		    0,                  // no extra data between each position
		    0                   // offset of first element
		  );


#if 1
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tex_ibo_elements);

		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
#else
		glDrawArrays(GL_TRIANGLES, 0, 18);
#endif

		glDisableVertexAttribArray(cube3dCoord3dAttribute);
		glDisableVertexAttribArray(cube3dUniformMytexture);


	}
}


/*
void YukigassenChunk::setupInternalRenderingDataPartial(long long voxelMapIndex)
{

	// Here all voxel maps in this room are iterated/looped so they can add their triangles and filaments.
	// TODO we need something more clever here so that we divide all these into the chunks.

	const int n=yukigassenVoxelMapList.capacity();
	for(int i=0; i<n;++i)
	{
		YukigassenVoxelMap *yvm=yukigassenVoxelMapList.getValueByIndex(i);
		if (yvm!=NULL)
		{
			if (yvm!=NULL)
			{
				yvm->setupInternalRenderingDataLocal(this);
			}
			else
			{
				printf("setupInternalRenderingData: Why was yvm NULL?\n");
			}
		}
		else
		{
			printf("setupInternalRenderingData: Why did this happen?\n");
		}
	}

}
*/

void YukigassenChunk::addYukigassenVoxelMap(YukigassenVoxelMap *yvm)
{
	//yukigassenVoxelMapList.put(yvm->geographicIndex, yvm->getId());
	yukigassenVoxelMapList.put(yvm->geographicIndex, yvm->getIndex());
}

