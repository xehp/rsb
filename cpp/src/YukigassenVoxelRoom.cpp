/*
YukigassenVoxelRoom.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


#include "includeGlEtc.h"
#include "HybridLib.h"

// #include <SOIL/SOIL.h> // http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06

#include "Pyramid3dProgram.h"
#include "D3Vector.h"
#include "D3Int.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"
#include "DrawText.h"
#include "ImageLoader.h"
#include "WordReader.h"
#include "MirrorDb.h"
#include "YukigassenChunk.h"
#include "YukigassenVoxelRoom.h"



YukigassenVoxelRoom::YukigassenVoxelRoom() :
YukigassenRoom(),
roomSizeInBlocks()
{
	// TODO Auto-generated constructor stub

}


YukigassenVoxelRoom::~YukigassenVoxelRoom() {
	// TODO Auto-generated destructor stub
}


void YukigassenVoxelRoom::readSelf(WordReader *wr)
{
	YukigassenRoom::readSelf(wr);


	roomSizeInBlocks.readSelf(wr);
}




float YukigassenVoxelRoom::blockSizeX() const
{
	return (roomSize.x / (float)roomSizeInBlocks.x);
}

float YukigassenVoxelRoom::blockSizeY() const
{
	return (roomSize.y / (float)roomSizeInBlocks.y);
}

float YukigassenVoxelRoom::blockSizeZ() const
{
	return (roomSize.z / (float)roomSizeInBlocks.z);
}

static int round_int( float r ) {
    return (r > 0.0) ? (r + 0.5f) : (r - 0.5f);
}

int YukigassenVoxelRoom::getGridFromPosX(float x) const
{
	return round_int(((x - blockSizeX()/2)*roomSizeInBlocks.x/roomSize.x));
}

int YukigassenVoxelRoom::getGridFromPosY(float y) const
{
	return round_int(((y - blockSizeY()/2)*roomSizeInBlocks.y/roomSize.y));
}

int YukigassenVoxelRoom::getGridFromPosZ(float z) const
{
	return round_int(((z - blockSizeZ()/2)*roomSizeInBlocks.z/roomSize.z));
}


size_t YukigassenVoxelRoom::translateGridToIndexX(int x) const
{
	return x+roomSizeInBlocks.x/2;
}

size_t YukigassenVoxelRoom::translateGridToIndexY(int y) const
{
	return y+roomSizeInBlocks.y/2;
}

size_t YukigassenVoxelRoom::translateGridToIndexZ(int z) const
{
	return z+roomSizeInBlocks.z/2;
}


long long YukigassenVoxelRoom::getIndexForPos(const D3Vector* posInRoom) const
{

	// translatePosToXyzGrid (grid is not same as index)
	const int gx=getGridFromPosX(posInRoom->x);
	const int gy=getGridFromPosY(posInRoom->y);
	const int gz=getGridFromPosZ(posInRoom->z);

	// translate from grid to index
	const int ix=translateGridToIndexX(gx);
	const int iy=translateGridToIndexY(gy);
	const int iz=translateGridToIndexZ(gz);

	// Check that is it within range
	if ((ix<0) || (ix>=roomSizeInBlocks.x) ||
		(iy<0) || (iy>=roomSizeInBlocks.y) ||
		(iz<0) || (iz>=roomSizeInBlocks.z))
	{
		return -1;  // Or use SIZE_MAX here.
	}

	long long  i = getIndexFromXYZ(ix, iy, iz);

	return i;
}




float YukigassenVoxelRoom::translateGridToCoordinatesZ(int z) const
{
	const float zSize = blockSizeZ();
	return ((z + 0.5f) * zSize );
}


bool YukigassenVoxelRoom::isInsideBlock(const D3Vector *pos) const
{
	long long i = getIndexForPos(pos);
	if (getVoxelValueForIndex(i)!=YukigassenChunk::EmptyBlock)
	{
		return true;
	}
	return false;
}

// Returns true if inside a block in this room
bool YukigassenVoxelRoom::isInsideBlockMargin(const D3Vector *pos, float margin) const
{
	// Examine the 8 corners
	const float x=pos->x;
	const float y=pos->y;
	const float z=pos->z;
	D3Vector corner0(x-margin, y-margin, z-margin);
	D3Vector corner1(x-margin, y-margin, z+margin);
	D3Vector corner2(x-margin, y+margin, z-margin);
	D3Vector corner3(x-margin, y+margin, z+margin);
	D3Vector corner4(x+margin, y-margin, z-margin);
	D3Vector corner5(x+margin, y-margin, z+margin);
	D3Vector corner6(x+margin, y+margin, z-margin);
	D3Vector corner7(x+margin, y+margin, z+margin);

	const bool i = 	isInsideBlock(&corner0) ||
					isInsideBlock(&corner1) ||
					isInsideBlock(&corner2) ||
					isInsideBlock(&corner3) ||
					isInsideBlock(&corner4) ||
					isInsideBlock(&corner5) ||
					isInsideBlock(&corner6) ||
					isInsideBlock(&corner7);

	return i;
}












float YukigassenVoxelRoom::calcDistToGround(const D3Vector *pos) const
{

	const float px=pos->x;
	const float py=pos->y;
	const float pz=pos->z;

	float dist=0.0f;

	while(dist<3.0f)
	{

		D3Vector v(px, py, pz+dist); // TODO shall increment in direction of gravity, for now assumed to always be along z axis

		if (isInsideBlock(&v))
		{
			// Find out which block it is.

			//D3Int xyz = translatePosToXyzGrid(v);
			//D3Vector gridPos=translateGridToPos(xyz);
			int gz=getGridFromPosZ(v.z);
			float bz = translateGridToCoordinatesZ(gz);

			dist = (bz - blockSizeZ()*0.5f) - pz ;

			return dist;
		}


		dist+=blockSizeZ()*0.25f;
	}


	return dist;
}
