/*
YukigassenBlockRoom.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef YukigassenBlockRoom_H
#define YukigassenBlockRoom_H

#include <vector>
#include "HashMap.h"
#include "D3Int.h"
#include "YukigassenChunk.h"
#include "YukigassenVoxelRoom.h"





class YukigassenBlockRoom: public YukigassenVoxelRoom
{
public:

	YukigassenBlockRoom();
	virtual ~YukigassenBlockRoom();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif
	virtual void setupInternalRenderingData();

	virtual const char* getTypeName() const {return "YukigassenBlockRoom";};


	virtual int getVoxelValueForIndex(long long index) const;

	virtual void renderSelf();

	// Translate a block index to voxel map index for x, y & z.
	inline int translateIndexToVoxelMapIndexX(int indexX) const {return indexX / (roomSizeInBlocks.x/roomSizeInVoxelMapsX);}
	inline int translateIndexToVoxelMapIndexY(int indexY) const {return indexY / (roomSizeInBlocks.y/roomSizeInVoxelMapsY);}
	inline int translateIndexToVoxelMapIndexZ(int indexZ) const {return indexZ / (roomSizeInBlocks.z/roomSizeInVoxelMapsZ);}

	// Translate a block index to the index inside a voxel map for x, y & z.
	inline int translateIndexToIndexInsideVoxelMapX(int x) const {return x % (roomSizeInBlocks.x/roomSizeInVoxelMapsX);}
	inline int translateIndexToIndexInsideVoxelMapY(int y) const {return y % (roomSizeInBlocks.y/roomSizeInVoxelMapsY);}
	inline int translateIndexToIndexInsideVoxelMapZ(int z) const {return z % (roomSizeInBlocks.z/roomSizeInVoxelMapsZ);}

	// Gives X, Y or Z part of index from one index on the sub room (AKA voxel map) level (not on the low block level)
	inline int translateToVoxelMapIndexX(long geoVoxemMapIndex) const {return (int)(geoVoxemMapIndex)%roomSizeInVoxelMapsX;}
	inline int translateToVoxelMapIndexY(long geoVoxemMapIndex) const {return (int)(geoVoxemMapIndex/roomSizeInVoxelMapsX)%roomSizeInVoxelMapsY;}
	inline int translateToVoxelMapIndexZ(long geoVoxemMapIndex) const {return (int)(geoVoxemMapIndex/(roomSizeInVoxelMapsX * roomSizeInVoxelMapsY))%roomSizeInVoxelMapsZ;}



	// TODO rename to getGeoSubIndexFromXyz
	inline long long getVoxelMapIndex(int subX, int subY, int subZ) const {return (long long)subX + (roomSizeInVoxelMapsX * (long long)subY) + ((roomSizeInVoxelMapsX * roomSizeInVoxelMapsY) * (long long)subZ);};

	YukigassenVoxelMap* getVoxelMap(int subX, int subY, int subZ) const;


	float translateBlockToCoordinatesX(int x) const;
	float translateBlockToCoordinatesY(int y) const;
	float translateBlockToCoordinatesZ(int z) const;




	virtual void regNamedChildObject(MirrorBase *addedObject);


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenBlockRoom(const YukigassenBlockRoom&);
	YukigassenBlockRoom& operator=(const YukigassenBlockRoom&);


	// How many sub rooms this is divided into for each dimension.
	int roomSizeInVoxelMapsX;
	int roomSizeInVoxelMapsY;
	int roomSizeInVoxelMapsZ;


	HashMap_64_32 voxelMapTable;

	YukigassenChunk yukigassenChunk;

};

#endif
