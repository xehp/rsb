/*
YukigassenVoxelRoom.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef YukigassenVoxelRoom_H
#define YukigassenVoxelRoom_H


#include "D3Int.h"
#include "YukigassenRoom.h"


const size_t FLOATS_PER_COLOR_VERTEX = 6;
const size_t COLOR_VERTICES_PER_LINE = 2;
const size_t FLOATS_PER_TEX_VERTEX = 3;
const size_t FLOATS_PER_TEX_COORD = 2;
const size_t SHORTS_PER_ELEMENT = 3;

class YukigassenVoxelMap;

class YukigassenVoxelRoom : public YukigassenRoom {
public:
	YukigassenVoxelRoom();
	virtual ~YukigassenVoxelRoom();


	virtual void readSelf(WordReader *wr);
	//virtual void setupInternalRenderingData();
	virtual void renderSelf()=0;

	virtual const char* getTypeName() const {return "YukigassenVoxelRoom";};

	virtual bool isInsideBlockMargin(const D3Vector *pos, float margin) const;
	virtual float calcDistToGround(const D3Vector *currentPos) const;

	virtual int getVoxelValueForIndex(long long index) const = 0; // this index identifies the voxel/block, it is not voxelmap index or chunk index.


	float blockSizeX() const;
	float blockSizeY() const;
	float blockSizeZ() const;
	int getGridFromPosX(float x) const;
	int getGridFromPosY(float y) const;
	int getGridFromPosZ(float z) const;
	size_t translateGridToIndexX(int x) const;
	size_t translateGridToIndexY(int y) const;
	size_t translateGridToIndexZ(int z) const;
	bool isInsideBlock(const D3Vector *pos) const;
	long long getIndexForPos(const D3Vector* pos) const;
	float translateGridToCoordinatesZ(int z) const;



	// Gives X, Y or Z part of index from one index on the block level (not the voxel map level)
	inline int getXFromIndex(long long index) const {return index % (roomSizeInBlocks.x);}
	inline int getYFromIndex(long long index) const {return (index / (roomSizeInBlocks.x)) % roomSizeInBlocks.y;}
	inline int getZFromIndex(long long index) const {return (index / (roomSizeInBlocks.x * roomSizeInBlocks.y));}

	inline long long getIndexFromXYZ(size_t x, size_t y, size_t z) const {const long long index = x + roomSizeInBlocks.x * y + roomSizeInBlocks.x * roomSizeInBlocks.y * z; return index;}
	int getVoxel(size_t x, size_t y, size_t z) const {const size_t index = getIndexFromXYZ(x, y, z); return getVoxelValueForIndex(index);}







	// TODO: Unfortunately the naming here is wrong.
	enum NeighborMask
	{
		MaskXp = 0x01,
		MaskYp = 0x02,
		MaskZp = 0x04,
		MaskXn = 0x08,  // This is actually Xp
		MaskYn = 0x10,  // This is actually Yp
		MaskZn = 0x20   // This is actually Zp
	};

	/*
	size_t getVoxelXp(size_t x, size_t y, size_t z) {return (x>0) ? getVoxel(x-1, y, z) : -1;}
	size_t getVoxelYp(size_t x, size_t y, size_t z) {return (y>0) ? getVoxel(x, y-1, z) : -1;}
	size_t getVoxelZp(size_t x, size_t y, size_t z) {return (z>0) ? getVoxel(x, y, z-1) : -1;}
	size_t getVoxelXn(size_t x, size_t y, size_t z) {return (x<(roomSizeInBlocks.x-1)) ? getVoxel(x+1, y, z) : -1;}
	size_t getVoxelYn(size_t x, size_t y, size_t z) {return (y<(roomSizeInBlocks.y-1)) ? getVoxel(x, y+1, z) : -1;}
	size_t getVoxelZn(size_t x, size_t y, size_t z) {return (z<(roomSizeInBlocks.z-1)) ? getVoxel(x, y, z+1) : -1;}
	*/

	/*
	size_t getIndexXp(size_t x, size_t y, size_t z) {return (x>0) ? getIndex(x-1, y, z) : -1;}
	size_t getIndexYp(size_t x, size_t y, size_t z) {return (y>0) ? getIndex(x, y-1, z) : -1;}
	size_t getIndexZp(size_t x, size_t y, size_t z) {return (z>0) ? getIndex(x, y, z-1) : -1;}
	size_t getIndexXn(size_t x, size_t y, size_t z) {return (x<(roomSizeInBlocks.x-1)) ? getIndex(x+1, y, z) : -1;}
	size_t getIndexYn(size_t x, size_t y, size_t z) {return (y<(roomSizeInBlocks.y-1)) ? getIndex(x, y+1, z) : -1;}
	size_t getIndexZn(size_t x, size_t y, size_t z) {return (z<(roomSizeInBlocks.z-1)) ? getIndex(x, y, z+1) : -1;}
	*/


	size_t getNeighborMaskXp(size_t index) const {return getVoxelValueForIndex(index)? MaskXp : 0;};
	size_t getNeighborMaskYp(size_t index) const {return getVoxelValueForIndex(index)? MaskYp : 0;};
	size_t getNeighborMaskZp(size_t index) const {return getVoxelValueForIndex(index)? MaskZp : 0;};
	size_t getNeighborMaskXn(size_t index) const {return getVoxelValueForIndex(index)? MaskXn : 0;};
	size_t getNeighborMaskYn(size_t index) const {return getVoxelValueForIndex(index)? MaskYn : 0;};
	size_t getNeighborMaskZn(size_t index) const {return getVoxelValueForIndex(index)? MaskZn : 0;};


	// TODO These are incorrectly named
	size_t getNeighborMaskXp(size_t x, size_t y, size_t z) const {return (x>0) ? getNeighborMaskXp(getIndexFromXYZ(x-1, y, z)) : 0;}
	size_t getNeighborMaskYp(size_t x, size_t y, size_t z) const {return (y>0) ? getNeighborMaskYp(getIndexFromXYZ(x, y-1, z)) : 0;}
	size_t getNeighborMaskZp(size_t x, size_t y, size_t z) const {return (z>0) ? getNeighborMaskZp(getIndexFromXYZ(x, y, z-1)) : 0;}
	size_t getNeighborMaskXn(size_t x, size_t y, size_t z) const {return (x<(roomSizeInBlocks.x-1)) ? getNeighborMaskXn(getIndexFromXYZ(x+1, y, z)) : 0;}
	size_t getNeighborMaskYn(size_t x, size_t y, size_t z) const {return (y<(roomSizeInBlocks.y-1)) ? getNeighborMaskYn(getIndexFromXYZ(x, y+1, z)) : 0;}
	size_t getNeighborMaskZn(size_t x, size_t y, size_t z) const {return (z<(roomSizeInBlocks.z-1)) ? getNeighborMaskZn(getIndexFromXYZ(x, y, z+1)) : 0;}


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenVoxelRoom(const YukigassenVoxelRoom&);
	YukigassenVoxelRoom& operator=(const YukigassenVoxelRoom&);

/*
	GLuint linesVertexBufferId;
	int linesNVertexesToRender;
	size_t linesVertexBufferSize; // size is in number of vertices it can store (not bytes), each line is FLOATS_PER_COLOR_VERTEX*COLOR_VERTICES_PER_LINE float
	std::vector<float> linesVertexBufferVector; // Here we store pairs of floats that make up lines (AKA filaments) that we will then move into GL buffer by calling glBufferData.


	GLuint triangleVertexBufferId;
	int triangleNVertexesToRender;
	size_t triangleVertexBufferSize; // size is in number of vertices it can store (not bytes), each line is FLOATS_PER_COLOR_VERTEX*COLOR_VERTICES_PER_LINE float
	std::vector<float> triangleVertexBufferVector; // Same as linesVertexBufferVector but for triangles.
*/

	D3Int roomSizeInBlocks;

};

#endif /* YUKIGASSENVOXELROOM_H_ */
