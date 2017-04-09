/*
 * YukigassenVoxelMap.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: May 1, 2015
 *      Author: henrik
 */

#ifndef YukigassenVoxelMap_H_
#define YukigassenVoxelMap_H_

#include "D3Int.h"
#include "MirrorContainer.h"

class WordReader;
class YukigassenChunk;
class YukigassenBlockRoom;
class YukigassenChunkRoom;

class YukigassenVoxelMap: public MirrorContainer {
public:
/*
	enum NeighborMask
	{
		MaskXp = 0x01,
		MaskYp = 0x02,
		MaskZp = 0x04,
		MaskXn = 0x08,
		MaskYn = 0x10,
		MaskZn = 0x20
	};
*/

	YukigassenVoxelMap();
	virtual ~YukigassenVoxelMap();

	inline int getVoxel(size_t index) {return voxelMap[index];}

	// Gives the index to the block inside of the voxel map (AKA sub room).
	inline size_t getBlockIndex(size_t x, size_t y, size_t z) {const size_t index = x + mapSizeInBlocks.x * y + mapSizeInBlocks.x * mapSizeInBlocks.y * z; return index;}

	int getVoxel(size_t x, size_t y, size_t z) {const size_t index = getBlockIndex(x, y, z); return voxelMap[index];}

	virtual void dataChanged(); // TODO What differs between dataChanged and setupInternalRenderingData. Why separate methods?
	virtual void setupInternalRenderingData();

	virtual void setupInternalRenderingDataLocal(const YukigassenBlockRoom *ybr, YukigassenChunk *yukigassenChunk);
	virtual void setupInternalRenderingDataLocal(const YukigassenChunkRoom *ybr, YukigassenChunk *yukigassenChunk);
	//virtual void setupInternalRenderingDataLocal(YukigassenChunk *yukigassenChunk);

	virtual const char* getTypeName() const {return "YukigassenVoxelMap";};

#ifdef NEIGBOUR_MAP
	inline int getNeighborMask(size_t index) {return neighbourMap[index];};
#endif

/*
	size_t getNeighborMaskXp(size_t index) {return getVoxel(index)? MaskXp : 0;};
	size_t getNeighborMaskYp(size_t index) {return getVoxel(index)? MaskYp : 0;};
	size_t getNeighborMaskZp(size_t index) {return getVoxel(index)? MaskZp : 0;};
	size_t getNeighborMaskXn(size_t index) {return getVoxel(index)? MaskXn : 0;};
	size_t getNeighborMaskYn(size_t index) {return getVoxel(index)? MaskYn : 0;};
	size_t getNeighborMaskZn(size_t index) {return getVoxel(index)? MaskZn : 0;};


	// These are incorrectly named
	size_t getNeighborMaskXp(size_t x, size_t y, size_t z) {return (x>0) ? getNeighborMaskXp(getBlockIndex(x-1, y, z)) : 0;}
	size_t getNeighborMaskYp(size_t x, size_t y, size_t z) {return (y>0) ? getNeighborMaskYp(getBlockIndex(x, y-1, z)) : 0;}
	size_t getNeighborMaskZp(size_t x, size_t y, size_t z) {return (z>0) ? getNeighborMaskZp(getBlockIndex(x, y, z-1)) : 0;}
	size_t getNeighborMaskXn(size_t x, size_t y, size_t z) {return (x<(mapSizeInBlocks.x-1)) ? getNeighborMaskXn(getBlockIndex(x+1, y, z)) : 0;}
	size_t getNeighborMaskYn(size_t x, size_t y, size_t z) {return (y<(mapSizeInBlocks.y-1)) ? getNeighborMaskYn(getBlockIndex(x, y+1, z)) : 0;}
	size_t getNeighborMaskZn(size_t x, size_t y, size_t z) {return (z<(mapSizeInBlocks.z-1)) ? getNeighborMaskZn(getBlockIndex(x, y, z+1)) : 0;}
*/

	long long geographicIndex;  // geographic index, this tells where in the block room this voxel map is located.
	D3Int mapSizeInBlocks; // This tells how large this voxel map is.

	char *voxelMap;
#ifdef NEIGBOUR_MAP
	char *neighbourMap;
#endif

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenVoxelMap(const YukigassenVoxelMap&);
	YukigassenVoxelMap& operator=(const YukigassenVoxelMap&);

};

#endif /* YukigassenVoxelMap_H_ */
