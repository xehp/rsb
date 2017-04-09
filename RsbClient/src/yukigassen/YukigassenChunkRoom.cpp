/*
YukigassenChunkRoom.cpp

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
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "YukigassenVoxelMap.h"
#include "YukigassenChunk.h"
#include "YukigassenBlockRoom.h"
#include "YukigassenChunkRoom.h"


// The difference between YukigassenBlockRoom and YukigassenChunkRoom is that YukigassenBlockRoom has one chunk while YukigassenChunkRoom has many.


YukigassenChunkRoom::YukigassenChunkRoom() :
	YukigassenVoxelRoom(),
	roomSizeInVoxelMapsX(8),
	roomSizeInVoxelMapsY(4),
	roomSizeInVoxelMapsZ(2),
	voxelMapTable(),
	chunkTable(),
	counter(0)
{

	calcRoomSizeInChunks();
}

// x is an index, from 0 and up to (not including) roomSizeInBlocks.x
float YukigassenChunkRoom::translateBlockToCoordinatesX(int x) const
{
	const float xSize = blockSizeX();
	return (x * xSize ) - (0.5f * roomSize.x) + (0.5f * xSize);
}

// y is an index, from 0 and up to (not including) roomSizeInBlocks.y
float YukigassenChunkRoom::translateBlockToCoordinatesY(int y) const
{
	const float ySize = blockSizeY();
	return (y * ySize ) - (0.5f * roomSize.y) + (0.5f * ySize);
}

// z is an index, from 0 and up to (not including) roomSizeInBlocks.z
float YukigassenChunkRoom::translateBlockToCoordinatesZ(int z) const
{
	const float zSize = blockSizeZ();
	return (z * zSize ) - (0.5f * roomSize.z) + (0.5f * zSize);
}

float YukigassenChunkRoom::translateChunkToCoordinatesX(int x) const
{
	const float xSize = chunkSizeX();
	return (x * xSize ) - (0.5f * roomSize.x) + (0.5f * xSize);
}

float YukigassenChunkRoom::translateChunkToCoordinatesY(int y) const
{
	const float ySize = chunkSizeY();
	return (y * ySize ) - (0.5f * roomSize.y) + (0.5f * ySize);
}

float YukigassenChunkRoom::translateChunkToCoordinatesZ(int z) const
{
	const float zSize = chunkSizeZ();
	return (z * zSize ) - (0.5f * roomSize.z) + (0.5f * zSize);
}



void YukigassenChunkRoom::setupInternalRenderingDataPartial(long long voxelMapIndex)
{
	char str[64];
	sprintf(str," ~%d %lld\n", this->id, voxelMapIndex);
	mainDebug("YukigassenChunkRoom::setupInternalRenderingData", str);



	// For this voxelMapIndex we need to look at in which chunk rooms the neighboring voxel maps are located since they may need updating also. In addition to self there are are 6 neighbors to consider, so 7 in total.

	const int x = translateToVoxelMapIndexX(voxelMapIndex);
	const int y = translateToVoxelMapIndexY(voxelMapIndex);
	const int z = translateToVoxelMapIndexZ(voxelMapIndex);

	examineUpdateNeed(x,y,z);
	examineUpdateNeed(x,y,z-1);
	examineUpdateNeed(x,y,z+1);
	examineUpdateNeed(x,y-1,z);
	examineUpdateNeed(x,y+1,z);
	examineUpdateNeed(x-1,y,z);
	examineUpdateNeed(x+1,y,z);


	updateIfNeeded(x,y,z);
	updateIfNeeded(x,y,z-1);
	updateIfNeeded(x,y,z+1);
	updateIfNeeded(x,y-1,z);
	updateIfNeeded(x,y+1,z);
	updateIfNeeded(x-1,y,z);
	updateIfNeeded(x+1,y,z);
}

void YukigassenChunkRoom::examineUpdateNeed(int x, int y, int z)
{
	long long vi = getVoxelMapIndex(x, y, z);
	long long ci=translateVoxelMapIndexToChunkIndex(vi);
	YukigassenChunk *yc = chunkTable.get(ci);
	if (yc!=NULL)
	{
		yc->needUpdate=true;
	}
}

void YukigassenChunkRoom::updateIfNeeded(int x, int y, int z)
{
	long long vi = getVoxelMapIndex(x, y, z);
	long long ci=translateVoxelMapIndexToChunkIndex(vi);
	YukigassenChunk *yc = chunkTable.get(ci);
	if (yc!=NULL)
	{
		if (yc->needUpdate)
		{
			setupInternalRenderingDataChunk(yc);
			yc->needUpdate=false;
		}
	}
}

void YukigassenChunkRoom::setupInternalRenderingDataChunk(YukigassenChunk *yukigassenChunk)
{
	assert(yukigassenChunk!=NULL);

	yukigassenChunk->reset();

	// These lines draws a line around each chunk in the room, it is used for debugging, to be removed (comment out) later.
	const long long chunkIndex=yukigassenChunk->chunkIndex;
	const int cx = translateToChunkIndexX(chunkIndex);
	const int cy = translateToChunkIndexY(chunkIndex);
	const int cz = translateToChunkIndexZ(chunkIndex);
	const float chunkPosX = translateChunkToCoordinatesX(cx);
	const float chunkPosY = translateChunkToCoordinatesY(cy);
	const float chunkPosZ = translateChunkToCoordinatesZ(cz);
	const float rsxHalf = (0.5f * roomSize.x)/this->roomSizeInChunksX;
	const float rsyHalf = (0.5f * roomSize.y)/this->roomSizeInChunksY;
	const float rszHalf = (0.5f * roomSize.z)/this->roomSizeInChunksZ;
	yukigassenChunk->addLineBox(chunkPosX, chunkPosY, chunkPosZ, rsxHalf, rsyHalf, rszHalf);


    const int n=yukigassenChunk->yukigassenVoxelMapList.capacity(); // How many geographic indexed do we have.
	for(int i=0; i<n;++i)
	{
		const int childIdx=yukigassenChunk->yukigassenVoxelMapList.getValueByIndex(i); // in block room using indexFromGeo here
		if (childIdx>=0)
		{
			MirrorBase *mb=this->getObjFromIndex(childIdx);
			if (mb!=NULL)
			{
				YukigassenVoxelMap *yvm=dynamic_cast<YukigassenVoxelMap *>(mb);
				if (yvm!=NULL)
				{
					yvm->setupInternalRenderingDataLocal(this, yukigassenChunk);
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


	yukigassenChunk->setup();






};





YukigassenChunkRoom::~YukigassenChunkRoom()
{
	// Make sure to free data if any.

	for(int i=0; i<chunkTable.capacity();++i)
	{
		YukigassenChunk *yc=chunkTable.getValueByIndex(i);
		if (yc!=NULL)
		{
			delete yc;
		}
	}
};





void YukigassenChunkRoom::readSelf(WordReader *wr)
{
	YukigassenVoxelRoom::readSelf(wr);

	roomSizeInVoxelMapsX = wr->readInt();
	roomSizeInVoxelMapsY = wr->readInt();
	roomSizeInVoxelMapsZ = wr->readInt();

	calcRoomSizeInChunks();
}

#ifndef __EMSCRIPTEN__
void YukigassenChunkRoom::writeSelf(WordWriter *ww)
{
	YukigassenVoxelRoom::writeSelf(ww);

   	ww->writeInt(roomSizeInVoxelMapsX);
   	ww->writeInt(roomSizeInVoxelMapsY);
   	ww->writeInt(roomSizeInVoxelMapsZ);
}
#endif


void YukigassenChunkRoom::renderSelf()
{
	// This is deprecated. Here we add all chunks for rendering. But we could exclude those out of view (such as to far away or behind the player) to reduce the workload for the GPU. The problem is that the avatar position is not known here. So method renderChunkRoom was added to do that.
	for(int i=0; i<chunkTable.capacity();++i)
	{
		YukigassenChunk *yc=chunkTable.getValueByIndex(i);
		if (yc!=NULL)
		{
			yc->renderSelf();
		}
	}
}


void YukigassenChunkRoom::renderChunkRoom(const D3PosAndOrientation *renderPos)
{
	// TODO this is the alternative to render parts of a chunk room
	// It is not implemented yet...

	/*if ((counter/60)%2==0)
	{
		renderAll();
	}

	else*/
	{
		// Now figure out in which chunk we are.

		const long long blockIndex = getIndexForPos(&renderPos->pos);

		const long long vmi = getVoxelMapIndexForIndex(blockIndex);

		const long long chunkIndex = translateVoxelMapIndexToChunkIndex(vmi);

		const int cx = translateToChunkIndexX(chunkIndex);
		const int cy = translateToChunkIndexY(chunkIndex);
		const int cz = translateToChunkIndexZ(chunkIndex);

		const int renderDist=2;

		// Here we loop the nearest chunks. TODO This is not optimal. We should render those chunks nearest camera first and then outwards (early-z-rejection). No need to process those behind.
		for(int x=cx-renderDist;x<=cx+renderDist;x++)
		{
			for(int y=cy-renderDist;y<=cy+renderDist;y++)
			{
				for(int z=cz-renderDist;z<=cz+renderDist;z++)
				{
					const long long chunkIndex2 = getChunkIndex(x,y,z);

					YukigassenChunk *yc=chunkTable.get(chunkIndex2);
					if (yc!=NULL)
					{
						yc->renderSelf();
					}

				}

			}

		}



#if 0
		for(int i=0; i<chunkTable.capacity();++i)
		{
			YukigassenChunk *yc=chunkTable.getValueByIndex(i);
			if (yc!=NULL)
			{
				yc->renderSelf();
			}
		}

#else

		// For now render all child objects, later only those near pos shall be rendered.
		for(int i=0; i<childVector.size();++i)
		{
			MirrorBase* child = childVector[i];
			if (child!=NULL)
			{
				child->renderAll();
			}
		}
#endif
	}

	++counter;
}


YukigassenVoxelMap* YukigassenChunkRoom::getVoxelMap(int subX, int subY, int subZ) const
{
	const long long geoSubIdx = getVoxelMapIndex(subX,subY,subZ);
	int i;


	i = voxelMapTable.get(geoSubIdx); // in block room using indexFromGeo here
	if (i>=0)
	{
		YukigassenVoxelMap *ysr=dynamic_cast<YukigassenVoxelMap *>(getObjFromIndex(i));
		return ysr;
	}


	return NULL;
}


long long YukigassenChunkRoom::getVoxelMapIndexForIndex(long long index) const
{
	if (index==-1)
	{
		return -1;
	}

	const int x = getXFromIndex(index);
	const int y = getYFromIndex(index);
	const int z = getZFromIndex(index);

	int vmiX = translateIndexToVoxelMapIndexX(x);
	int vmiY = translateIndexToVoxelMapIndexY(y);
	int vmiZ = translateIndexToVoxelMapIndexZ(z);

	long long vmi = getVoxelMapIndex(vmiX, vmiY, vmiZ);



	return vmi;

}


int YukigassenChunkRoom::getVoxelValueForIndex(long long index) const
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
D3Int YukigassenChunkRoom::translatePosToXyzGrid(final D3Vector posInRoom)
{
	//D3Int xyz=new D3Int((int)(((posInRoom.x)*roomSizeInBlocks.x/size.x)), (int)(((posInRoom.y)*roomSizeInBlocks.y/size.y)), (int)(((posInRoom.z)*roomSizeInBlocks.z/size.z)));
	final D3Int xyz=new D3Int(Math.round(((posInRoom.x-blockSizeX()/2)*roomSizeInBlocks.x/roomSize.x)), Math.round(((posInRoom.y-blockSizeY()/2)*roomSizeInBlocks.y/roomSize.y)), Math.round(((posInRoom.z-blockSizeZ()/2)*roomSizeInBlocks.z/roomSize.z)));
	//debug("translatePosToXyzGrid " +posInRoom+"  "+xyz);
	return xyz;
}


// Translate an integer grid pos to index
// Grid and index differ in the way that grid pos 0,0,0 is in the middle of the room (origo) while index (0,0,0) is in one corner.
D3Int YukigassenChunkRoom::translateGridPosToXyzIndex(final D3Int xyzGrid)
{
	final D3Int xyzIndex=new D3Int(xyzGrid.x+roomSizeInBlocks.x/2, xyzGrid.y+roomSizeInBlocks.y/2, xyzGrid.z+roomSizeInBlocks.z/2);
	return xyzIndex;
}
*/

void YukigassenChunkRoom::regNamedChildObject(MirrorBase *addedObject)
{

	YukigassenVoxelMap *yvm=dynamic_cast<YukigassenVoxelMap *>(addedObject);

	if (yvm!=NULL)
	{
		// We need to get sub rooms added to our list of sub rooms
		voxelMapTable.put(yvm->geographicIndex, yvm->index);


		long long ci = translateVoxelMapIndexToChunkIndex(yvm->geographicIndex);
		YukigassenChunk *yc = getOrCreateChunk(ci);

		yc->addYukigassenVoxelMap(yvm);

	}
	else
	{
		// Perhaps super class knows what to do with this one?
		YukigassenRoom::regNamedChildObject(addedObject);
	}

}

void YukigassenChunkRoom::calcRoomSizeInChunks()
{
	roomSizeInChunksX=(roomSizeInVoxelMapsX+3)/4;
	roomSizeInChunksY=(roomSizeInVoxelMapsY+3)/4;
	roomSizeInChunksZ=(roomSizeInVoxelMapsZ+3)/4;
}


YukigassenChunk *YukigassenChunkRoom::getOrCreateChunk(long long chunkIndex)
{
	YukigassenChunk *yc = chunkTable.get(chunkIndex);

	if (yc==NULL)
	{
		yc = new YukigassenChunk(chunkIndex); // Remember to delete in YukigassenChunkRoom::~YukigassenChunkRoom()
		chunkTable.put(chunkIndex, yc);
	}

	return yc;
}


long long YukigassenChunkRoom::translateVoxelMapIndexToChunkIndex(long long geographicVoxelMapIndex) const
{
	// To what chunk does this voxel map belong?
	int vx = translateToVoxelMapIndexX(geographicVoxelMapIndex);
	int vy = translateToVoxelMapIndexY(geographicVoxelMapIndex);
	int vz = translateToVoxelMapIndexZ(geographicVoxelMapIndex);

	assert(roomSizeInVoxelMapsX!=0);
	assert(getChunkSizeInVoxelMapsX()!=0);
	int cx = getChunkIndexForVoxelMapIndexX(vx);
	int cy = getChunkIndexForVoxelMapIndexY(vy);
	int cz = getChunkIndexForVoxelMapIndexZ(vz);

	long long ci = getChunkIndex(cx, cy, cz);

	return ci;
}

