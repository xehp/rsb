/*
 * YukigassenVoxelMap.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: May 1, 2015
 *      Author: henrik
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "WordReader.h"
#include "YukigassenChunk.h"
#include "YukigassenVoxelRoom.h"
#include "YukigassenChunkRoom.h"
#include "YukigassenBlockRoom.h"
#include "YukigassenVoxelMap.h"

YukigassenVoxelMap::YukigassenVoxelMap() :
	MirrorContainer(),
	geographicIndex(-1),
	mapSizeInBlocks(0,0,0),
	voxelMap(NULL)
#ifdef NEIGBOUR_MAP
	neighbourMap(NULL),
#endif
{
}


YukigassenVoxelMap::~YukigassenVoxelMap() {
	free(voxelMap);
#ifdef NEIGBOUR_MAP
	free(neighbourMap);
#endif
}


void YukigassenVoxelMap::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	free(voxelMap); // Free old data area if any
#ifdef NEIGBOUR_MAP
	free(neighbourMap); // Free old data area if any
#endif

	geographicIndex = wr->readLong();

	mapSizeInBlocks.readSelf(wr);

	size_t voxelMapSize = mapSizeInBlocks.x * mapSizeInBlocks.y * mapSizeInBlocks.z;

	voxelMap = (char *)malloc(voxelMapSize); // free is done in destructor free(voxelMap);
	assert(voxelMap!=NULL);
#ifdef NEIGBOUR_MAP
	neighbourMap = (char *)malloc(voxelMapSize);
	assert(neighbourMap!=NULL);
#endif

    for(size_t x=0;x<mapSizeInBlocks.x;x++)
    {
        for(size_t y=0;y<mapSizeInBlocks.y;y++)
        {
            for(size_t z=0;z<mapSizeInBlocks.z;z++)
            {
            	const int b=wr->readInt();
            	const int index = getBlockIndex(x,y,z);
            	voxelMap[index]=b;
            }
        }
    }

#ifdef NEIGBOUR_MAP
    // Calculate neighbourMask
    for(size_t x=0;x<mapSizeInBlocks.x;x++)
    {
        for(size_t y=0;y<mapSizeInBlocks.y;y++)
        {
            for(size_t z=0;z<mapSizeInBlocks.z;z++)
            {
            	const int index = getBlockIndex(x,y,z);

            	neighbourMap[index] = getNeighborMaskXp(x,y,z) | getNeighborMaskYp(x,y,z) | getNeighborMaskZp(x,y,z) | getNeighborMaskXn(x,y,z) | getNeighborMaskYn(x,y,z) | getNeighborMaskZn(x,y,z);
            }
        }
    }
#endif
}


void YukigassenVoxelMap::dataChanged()
{
	MirrorContainer::dataChanged();

	/*
	YukigassenBlockRoom *ybr=dynamic_cast<YukigassenBlockRoom *>(getParentObj());
	if (ybr!=NULL)
	{
		ybr->setupInternalRenderingData();
	}*/
	setupInternalRenderingData();

}


void YukigassenVoxelMap::setupInternalRenderingData()
{
	YukigassenBlockRoom *ybr=dynamic_cast<YukigassenBlockRoom *>(getParentObj());
	if (ybr!=NULL)
	{
		ybr->setupInternalRenderingData();
	}
	else
	{
		YukigassenChunkRoom *ycr=dynamic_cast<YukigassenChunkRoom *>(getParentObj());
		if (ycr!=NULL)
		{
			ycr->setupInternalRenderingDataPartial(this->geographicIndex);
		}

	}
}


void YukigassenVoxelMap::setupInternalRenderingDataLocal(const YukigassenBlockRoom *ybr, YukigassenChunk *yukigassenChunk)
{
	assert(ybr==getParentObj());
	if (ybr!=NULL)
	{
		const float bsx2 = 0.5f * ybr->blockSizeX();
		const float bsy2 = 0.5f * ybr->blockSizeY();
		const float bsz2 = 0.5f * ybr->blockSizeZ();

		const int subXo=ybr->translateToVoxelMapIndexX(geographicIndex)*mapSizeInBlocks.x;
		const int subYo=ybr->translateToVoxelMapIndexY(geographicIndex)*mapSizeInBlocks.y;
		const int subZo=ybr->translateToVoxelMapIndexZ(geographicIndex)*mapSizeInBlocks.z;


		//addLineBox(0.0f, 0.0f, 0.0f, rsx2, rsy2, rsz2);

		//printf("setupInternalRenderingDataLocal: %d %d %d\n", subXo, subYo, subZo);

	    for(size_t lx=0;lx<mapSizeInBlocks.x;lx++)
	    {
	        for(size_t ly=0;ly<mapSizeInBlocks.y;ly++)
	        {
	            for(size_t lz=0;lz<mapSizeInBlocks.z;lz++)
	            {
	            	const long long blockIndex = getBlockIndex(lx,ly,lz);

					const int b = getVoxel(blockIndex);
					if (!YukigassenChunk::getBlockInvisible(b))
					{
						int rx=subXo+lx;
						int ry=subYo+ly;
						int rz=subZo+lz;

						// TODO Would be much faster to not do this via parent unless needed.
						int neighborMask = ybr->getNeighborMaskXp(rx,ry,rz) | ybr->getNeighborMaskYp(rx,ry,rz) | ybr->getNeighborMaskZp(rx,ry,rz) | ybr->getNeighborMaskXn(rx,ry,rz) | ybr->getNeighborMaskYn(rx,ry,rz) | ybr->getNeighborMaskZn(rx,ry,rz);
						//int neighborMask=0x1f;

						float xc = ybr->translateBlockToCoordinatesX(rx);
						float yc = ybr->translateBlockToCoordinatesY(ry);
						float zc = ybr->translateBlockToCoordinatesZ(rz);

						//printf("setupInternalRenderingDataLocal: %f %f %f  %03x %d\n", xc, yc, zc, neighborMask, b);
						//if ((bsx2==bsy2) && (bsy2==bsz2)) {};
						if (neighborMask!=0x3F)
						{
							// TODO If this bloc is same as the one next to it we could merge their surfaces into one surface. That would reduce the workload for the GPU.
							yukigassenChunk->addTriangleBox(xc, yc, zc, bsx2, bsy2, bsz2, neighborMask, b);
						}
	            	}
	            }
	        }
	    }

	}
}


// TODO YukigassenChunkRoom should probably pass a pointer to an object that can store the triangles and filaments instead of passing pointer to itself.
void YukigassenVoxelMap::setupInternalRenderingDataLocal(const YukigassenChunkRoom *ybr, YukigassenChunk *yukigassenChunk)
{
	assert(ybr==getParentObj());
/*void YukigassenVoxelMap::setupInternalRenderingDataLocal(YukigassenChunk *yukigassenChunk)
{
	const YukigassenChunkRoom *ybr=dynamic_cast<const YukigassenChunkRoom *>(getParentObj());*/
	if (ybr!=NULL)
	{
		const float bsx2 = 0.5f * ybr->blockSizeX();
		const float bsy2 = 0.5f * ybr->blockSizeY();
		const float bsz2 = 0.5f * ybr->blockSizeZ();

		const int subXo=ybr->translateToVoxelMapIndexX(geographicIndex)*mapSizeInBlocks.x;
		const int subYo=ybr->translateToVoxelMapIndexY(geographicIndex)*mapSizeInBlocks.y;
		const int subZo=ybr->translateToVoxelMapIndexZ(geographicIndex)*mapSizeInBlocks.z;


		//addLineBox(0.0f, 0.0f, 0.0f, rsx2, rsy2, rsz2);

		//printf("setupInternalRenderingDataLocal: %d %d %d\n", subXo, subYo, subZo);

	    for(size_t lx=0;lx<mapSizeInBlocks.x;lx++)
	    {
	        for(size_t ly=0;ly<mapSizeInBlocks.y;ly++)
	        {
	            for(size_t lz=0;lz<mapSizeInBlocks.z;lz++)
	            {
	            	const long long blockIndex = getBlockIndex(lx,ly,lz);

					const int b = getVoxel(blockIndex);
					if (!YukigassenChunk::getBlockInvisible(b))
					{
						int rx=subXo+lx;
						int ry=subYo+ly;
						int rz=subZo+lz;

						// TODO Would be much faster to not do this via parent unless needed.
						int neighborMask = ybr->getNeighborMaskXp(rx,ry,rz) | ybr->getNeighborMaskYp(rx,ry,rz) | ybr->getNeighborMaskZp(rx,ry,rz) | ybr->getNeighborMaskXn(rx,ry,rz) | ybr->getNeighborMaskYn(rx,ry,rz) | ybr->getNeighborMaskZn(rx,ry,rz);
						//int neighborMask=0x1f;

						float xc = ybr->translateBlockToCoordinatesX(rx);
						float yc = ybr->translateBlockToCoordinatesY(ry);
						float zc = ybr->translateBlockToCoordinatesZ(rz);

						//printf("setupInternalRenderingDataLocal: %f %f %f  %03x %d\n", xc, yc, zc, neighborMask, b);
						//if ((bsx2==bsy2) && (bsy2==bsz2)) {};
						if (neighborMask!=0x3F)
						{
							yukigassenChunk->addTriangleBox(xc, yc, zc, bsx2, bsy2, bsz2, neighborMask, b);
						}
	            	}
	            }
	        }
	    }

	}
}

