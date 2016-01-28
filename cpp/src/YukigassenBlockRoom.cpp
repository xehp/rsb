/*
YukigassenBlockRoom.cpp

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
#include "YukigassenVoxelMap.h"
#include "YukigassenChunkRoom.h"
#include "YukigassenBlockRoom.h"


// The difference between YukigassenBlockRoom and YukigassenChunkRoom is that YukigassenBlockRoom has one chunk while YukigassenChunkRoom has many.

YukigassenBlockRoom::YukigassenBlockRoom() :
	YukigassenVoxelRoom(),
	roomSizeInVoxelMapsX(8),
	roomSizeInVoxelMapsY(4),
	roomSizeInVoxelMapsZ(2),
	voxelMapTable(),
	yukigassenChunk(0)
{


}

// x is an index, from 0 and up to (not including) roomSizeInBlocks.x
float YukigassenBlockRoom::translateBlockToCoordinatesX(int x) const
{
	const float xSize = blockSizeX();
	return (x * xSize ) - (0.5f * roomSize.x) + (0.5f * xSize);
}

// y is an index, from 0 and up to (not including) roomSizeInBlocks.y
float YukigassenBlockRoom::translateBlockToCoordinatesY(int y) const
{
	const float ySize = blockSizeY();
	return (y * ySize ) - (0.5f * roomSize.y) + (0.5f * ySize);
}

// z is an index, from 0 and up to (not including) roomSizeInBlocks.z
float YukigassenBlockRoom::translateBlockToCoordinatesZ(int z) const
{
	const float zSize = blockSizeZ();
	return (z * zSize ) - (0.5f * roomSize.z) + (0.5f * zSize);
}



// In block room geo index is same as child index. This is the main difference between block rooms and chunk rooms.
/*
static inline int indexFromGeo(long geoIdx)
{
	assert(geoIdx <= 0x7FFFFFFFL);

	return (int)geoIdx;
}
*/

void YukigassenBlockRoom::setupInternalRenderingData()
{
	char str[32];
	sprintf(str," ~%d\n", this->id);
	mainDebug("YukigassenBlockRoom::setupInternalRenderingData", str);


	//YukigassenRoom::setupInternalRenderingData();



	yukigassenChunk.reset();



	const float rsx2 = 0.5f * roomSize.x;
	const float rsy2 = 0.5f * roomSize.y;
	const float rsz2 = 0.5f * roomSize.z;

	// This draws a line around the room (used for debugging)
	yukigassenChunk.addLineBox(0.0f, 0.0f, 0.0f, rsx2, rsy2, rsz2);


// (roomSize.x / (float)roomSizeInBlocks.x);


	// Here all voxel maps in this room are iterated/looped so they can add their triangles and filaments to the lists for this room.
    const int n=voxelMapTable.capacity(); // How many geographic indexed do we have.
	for(int i=0; i<n;++i)
	{
		const int childIdx=voxelMapTable.getValueByIndex(i); // in block room using indexFromGeo here
		if (childIdx>=0)
		{
			MirrorBase *mb=this->getObjFromIndex(childIdx);
			if (mb!=NULL)
			{
				YukigassenVoxelMap *yvm=dynamic_cast<YukigassenVoxelMap *>(mb);
				if (yvm!=NULL)
				{
					yvm->setupInternalRenderingDataLocal(this, &yukigassenChunk);
				}
				else
				{
					printf("setupInternalRenderingData: Why was yvm NULL? i=%d, childIdx=%d, n=%d\n", i, childIdx, n);
				}
			}
			else
			{
				printf("setupInternalRenderingData: Object not found, i=%d, childIdx=%d, n=%d\n", i, childIdx, n);
			}
		}
	}

	yukigassenChunk.setup();



};



/*
*/


YukigassenBlockRoom::~YukigassenBlockRoom()
{
};





void YukigassenBlockRoom::readSelf(WordReader *wr)
{
	YukigassenVoxelRoom::readSelf(wr);

	roomSizeInVoxelMapsX = wr->readInt();
	roomSizeInVoxelMapsY = wr->readInt();
	roomSizeInVoxelMapsZ = wr->readInt();
}


void YukigassenBlockRoom::renderSelf()
{
	yukigassenChunk.renderSelf();
}



YukigassenVoxelMap* YukigassenBlockRoom::getVoxelMap(int subX, int subY, int subZ) const
{
	const long long geoSubIdx = getVoxelMapIndex(subX,subY,subZ);
	int i;


#ifdef USE_STD_MAP
	std::map<long long, int>::iterator iter = voxelMapTable.find(geoSubIdx);
	if (iter != voxelMapTable.end() )
	{
	    // key 2 exists, do something with iter->second (the value)
		i = iter->second;
		YukigassenVoxelMap *ysr=dynamic_cast<YukigassenVoxelMap *>(getObjFromIndex(i));

		return ysr;
	}
#else
	i = voxelMapTable.get(geoSubIdx); // in block room using indexFromGeo here
	if (i>=0)
	{
		YukigassenVoxelMap *ysr=dynamic_cast<YukigassenVoxelMap *>(getObjFromIndex(i));
		return ysr;
	}

#endif

	return NULL;
}




int YukigassenBlockRoom::getVoxelValueForIndex(long long index) const
{
	if (index==-1)
	{
		return 0;
	}

	const int x = getXFromIndex(index);
	const int y = getYFromIndex(index);
	const int z = getZFromIndex(index);


	// subX/Y/Z = index telling which voxel map the block is in
	// remX/Y/Z = remainder, the less significant part of x index, tells which block within the voxel map it is.
	int subX = x / (roomSizeInBlocks.x/roomSizeInVoxelMapsX);
	int remX = x % (roomSizeInBlocks.x/roomSizeInVoxelMapsX);
	int subY = y / (roomSizeInBlocks.y/roomSizeInVoxelMapsY);
	int remY = y % (roomSizeInBlocks.y/roomSizeInVoxelMapsY);
	int subZ = z / (roomSizeInBlocks.z/roomSizeInVoxelMapsZ);
	int remZ = z % (roomSizeInBlocks.z/roomSizeInVoxelMapsZ);

	// TODO the some of the above is same as the below.
	assert(subX == translateIndexToVoxelMapIndexX(x));
	assert(subY == translateIndexToVoxelMapIndexY(y));
	assert(subZ == translateIndexToVoxelMapIndexZ(z));
	assert(remX == translateIndexToIndexInsideVoxelMapX(x));
	assert(remY == translateIndexToIndexInsideVoxelMapY(y));
	assert(remZ == translateIndexToIndexInsideVoxelMapZ(z));

	YukigassenVoxelMap *ysr = getVoxelMap(subX, subY, subZ);

	if (ysr == NULL)
	{
		return 0;
	}

	return ysr->getVoxel(remX, remY, remZ);

}


/*
// Translate a pos within the block to grid
// posInRoom is to be given in coordinates of this room
// The inverse of this is translateGridToPos.
// Note! this is not same as index in the voxelMap.
D3Int YukigassenBlockRoom::translatePosToXyzGrid(final D3Vector posInRoom)
{
	//D3Int xyz=new D3Int((int)(((posInRoom.x)*roomSizeInBlocks.x/size.x)), (int)(((posInRoom.y)*roomSizeInBlocks.y/size.y)), (int)(((posInRoom.z)*roomSizeInBlocks.z/size.z)));
	final D3Int xyz=new D3Int(Math.round(((posInRoom.x-blockSizeX()/2)*roomSizeInBlocks.x/roomSize.x)), Math.round(((posInRoom.y-blockSizeY()/2)*roomSizeInBlocks.y/roomSize.y)), Math.round(((posInRoom.z-blockSizeZ()/2)*roomSizeInBlocks.z/roomSize.z)));
	//debug("translatePosToXyzGrid " +posInRoom+"  "+xyz);
	return xyz;
}


// Translate an integer grid pos to index
// Grid and index differ in the way that grid pos 0,0,0 is in the middle of the room (origo) while index (0,0,0) is in one corner.
D3Int YukigassenBlockRoom::translateGridPosToXyzIndex(final D3Int xyzGrid)
{
	final D3Int xyzIndex=new D3Int(xyzGrid.x+roomSizeInBlocks.x/2, xyzGrid.y+roomSizeInBlocks.y/2, xyzGrid.z+roomSizeInBlocks.z/2);
	return xyzIndex;
}
*/

void YukigassenBlockRoom::regNamedObject(MirrorBase *addedObject)
{
	YukigassenVoxelMap *yvm=dynamic_cast<YukigassenVoxelMap *>(addedObject);

	if (yvm!=NULL)
	{
		// We need to get sub rooms added to our list of sub rooms
		voxelMapTable.put(yvm->geographicIndex, yvm->index);
	}
	else
	{
		// Perhaps super class knows what to do with this one?
		YukigassenRoom::regNamedObject(addedObject);
	}

}

