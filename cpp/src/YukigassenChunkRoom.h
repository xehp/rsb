/*
YukigassenChunkRoom.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef YUKIGASSENCHUNKROOM_H_
#define YUKIGASSENCHUNKROOM_H_


#include "HashMap.h"


#include <SDL_opengl.h>
#include "D3Int.h"
#include "YukigassenVoxelRoom.h"



class YukigassenChunk;


class YukigassenChunkRoom: public YukigassenVoxelRoom
{
public:
	YukigassenChunkRoom();
	virtual ~YukigassenChunkRoom();

	virtual void readSelf(WordReader *wr);
	virtual void setupInternalRenderingDataPartial(long long voxelMapIndex);
	virtual const char* getTypeName() const {return "YukigassenChunkRoom";};


	virtual long long getVoxelMapIndexForIndex(long long index) const;
	virtual int getVoxelValueForIndex(long long index) const; // overriding d.o. in YukigassenVoxelRoom
	virtual void renderSelf();
	virtual void regNamedObject(MirrorBase *addedObject);

	virtual void setupInternalRenderingDataChunk(YukigassenChunk *yukigassenChunk);

	virtual void renderChunkRoom(const D3PosAndOrientation *renderPos);

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

	// Gives chunk X/Y/Z index from chunk indexe. This is the inverse of getChunkIndex
	inline int translateToChunkIndexX(long chunkIndex) const {return (int)(chunkIndex)%roomSizeInChunksX;}
	inline int translateToChunkIndexY(long chunkIndex) const {return (int)(chunkIndex/roomSizeInChunksX)%roomSizeInChunksY;}
	inline int translateToChunkIndexZ(long chunkIndex) const {return (int)(chunkIndex/(roomSizeInChunksX * roomSizeInChunksY))%roomSizeInChunksZ;}

	void examineUpdateNeed(int x, int y, int z);
	void updateIfNeeded(int x, int y, int z);


	// Gives voxel map index from voxel map xyz indexes.
	inline long long getVoxelMapIndex(int subX, int subY, int subZ) const {return (long long)subX + (roomSizeInVoxelMapsX * (long long)subY) + ((roomSizeInVoxelMapsX * roomSizeInVoxelMapsY) * (long long)subZ);};

	// Gives chunk index from chunk xyz indexes. This is the inverse of translateToChunkIndexX/Y/Z
	inline long long getChunkIndex(int subX, int subY, int subZ) const {return (long long)subX + (roomSizeInChunksX * (long long)subY) + ((roomSizeInChunksX * roomSizeInChunksY) * (long long)subZ);};

	YukigassenVoxelMap* getVoxelMap(int subX, int subY, int subZ) const;


	float translateBlockToCoordinatesX(int x) const;
	float translateBlockToCoordinatesY(int y) const;
	float translateBlockToCoordinatesZ(int z) const;

	float translateChunkToCoordinatesX(int x) const;
	float translateChunkToCoordinatesY(int y) const;
	float translateChunkToCoordinatesZ(int z) const;

	inline int getChunkSizeInVoxelMapsX() const {return roomSizeInVoxelMapsX/roomSizeInChunksX;}
	inline int getChunkSizeInVoxelMapsY() const {return roomSizeInVoxelMapsY/roomSizeInChunksY;}
	inline int getChunkSizeInVoxelMapsZ() const {return roomSizeInVoxelMapsZ/roomSizeInChunksZ;}

	inline int getChunkIndexForVoxelMapIndexX(long voxelMapIndexX) const {return voxelMapIndexX/getChunkSizeInVoxelMapsX();}
	inline int getChunkIndexForVoxelMapIndexY(long voxelMapIndexY) const {return voxelMapIndexY/getChunkSizeInVoxelMapsY();}
	inline int getChunkIndexForVoxelMapIndexZ(long voxelMapIndexZ) const {return voxelMapIndexZ/getChunkSizeInVoxelMapsZ();}


	long long translateVoxelMapIndexToChunkIndex(long long geographicVoxelMapIndex) const;

	float chunkSizeX() const {return (float)roomSize.x/(float)roomSizeInChunksX;}
	float chunkSizeY() const {return (float)roomSize.y/(float)roomSizeInChunksY;}
	float chunkSizeZ() const {return (float)roomSize.z/(float)roomSizeInChunksZ;}



	void calcRoomSizeInChunks();
	YukigassenChunk *getOrCreateChunk(long long chunkIndex);


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenChunkRoom(const YukigassenChunkRoom&);
	YukigassenChunkRoom& operator=(const YukigassenChunkRoom&);


	// How many sub rooms this is divided into for each dimension.
	int roomSizeInVoxelMapsX;
	int roomSizeInVoxelMapsY;
	int roomSizeInVoxelMapsZ;

	int roomSizeInChunksX;
	int roomSizeInChunksY;
	int roomSizeInChunksZ;

	HashMap_64_32 voxelMapTable;

	HashMap_64_PTR<YukigassenChunk> chunkTable;

	int counter;
};

#endif
