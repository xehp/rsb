/*
YukigassenRoom.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include "includeGlEtc.h"
#include "HybridLib.h"

#ifndef __EMSCRIPTEN__
#include "hca.h"
#endif

#include "D3Vector.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"
#include "DrawText.h"
#include "ImageLoader.h"
#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif


#include "YukigassenRoom.h"




YukigassenRoom::YukigassenRoom() :
	D3ObjWithPos(),
	localTime(0),
	roomSize(),
	roomAcceleration(),
	yawLeftRate(),
	pitchUpRate(),
	rollLeftRate()

{
}






YukigassenRoom::~YukigassenRoom()
{
};



void YukigassenRoom::readSelf(WordReader *wr)
{
	D3ObjWithPos::readSelf(wr);

	localTime = wr->readLong(); // YukigassenActiveObject

	roomSize.readSelf(wr);

	roomAcceleration.readSelf(wr);

	yawLeftRate=wr->readFloat();
	pitchUpRate=wr->readFloat();
	rollLeftRate=wr->readFloat();


	/*if (!roomSize.equals(oldRoomSize))
	{
		setupInternalRenderingData();
		oldRoomSize = roomSize;
	}*/
}

#ifndef __EMSCRIPTEN__
void YukigassenRoom::writeSelf(WordWriter *ww)
{
	D3ObjWithPos::writeSelf(ww);

	//allowedMove.readSelf(wr);
	ww->writeLong(localTime);
	roomSize.writeSelf(ww);
	roomAcceleration.writeSelf(ww);
	ww->writeFloat(yawLeftRate);
	ww->writeFloat(pitchUpRate);
	ww->writeFloat(rollLeftRate);
}
#endif


/*
void YukigassenRoom::renderAll()
{
	// Read more at: https://gitlab.com/wikibooks-opengl/modern-tutorials/blob/master/tut06_textures/cube.cpp

	// TO DO: Probably we should translate and rotate before rendering child objects.
	MirrorContainer::renderAll();
}
*/

// Translates from coordinates of this room to global
void YukigassenRoom::translatePaoToGlobal(D3PosAndOrientation &pao) const
{
	const MirrorBase *parentRoomMb=this->getParentObj();
	const YukigassenRoom *parentRoomYr=dynamic_cast<const YukigassenRoom *>(parentRoomMb);

	if (parentRoomYr!=NULL)
	{
		this->pao.translatePaoToSuper(&pao);
		parentRoomYr->translatePaoToGlobal(pao);
	}
}


// This code is not tested.
// Is pos inside the room, inside with some margin (not only its center).
// pos shall be given in the coordinates of this room.
bool YukigassenRoom::isInsideRoomMargin(const D3Vector *pos, float margin) const
{
	float x=pos->x;
	float y=pos->x;
	float z=pos->x;
	float x2=roomSize.x/2;
	float y2=roomSize.y/2;
	float z2=roomSize.z/2;

	if ((x-margin)<-x2)
	{
		return false;
	}
	if ((y-margin)<-y2)
	{
		return false;
	}
	if ((z-margin)<-z2)
	{
		return false;
	}
	if ((x+margin)>x2)
	{
		return false;
	}
	if ((y+margin)>y2)
	{
		return false;
	}
	if ((z+margin)>z2)
	{
		return false;
	}
	return true;
}

// This is used when moving the avatar. We need to know if avatar is in the air or walking on ground
// If distance is larger than maxDistToCalculate it don't need to be calculated further.
float YukigassenRoom::calcDistToGround(const D3Vector *currentPos, const float maxDistToCalculate) const
{
	return 0;
}

