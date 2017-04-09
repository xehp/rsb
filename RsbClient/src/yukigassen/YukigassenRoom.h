/*
YukigassenRoom.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef YukigassenRoom_H
#define YukigassenRoom_H

#include <SDL_opengl.h>
#include "D3Vector.h"
#include "D3ObjWithPos.h"

class WordReader;

class YukigassenRoom: public D3ObjWithPos
{
public:
	YukigassenRoom();
	virtual ~YukigassenRoom();

	//virtual void renderAll();
	//virtual void renderSelf();
	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif
	virtual void setupInternalRenderingData() {};
	virtual void translatePaoToGlobal(D3PosAndOrientation &pao) const;

	virtual bool isInsideRoomMargin(const D3Vector *pos, float margin) const;
	virtual bool isInsideBlockMargin(const D3Vector *pos, float margin) const {return isInsideRoomMargin(pos, margin);}; // TODO what do we do here?
	virtual float calcDistToGround(const D3Vector *currentPos, const float maxDistToCalculate) const;

	long localTime;

	D3Vector roomSize;
	D3Vector roomAcceleration;  // It may be that we have sign wrong on this. As it is now it will tell how objects in the room accelerate (more like gravity) not how this room itself accelerates which is in the opposite direction.

	float yawLeftRate;
	float pitchUpRate;
	float rollLeftRate;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenRoom(const YukigassenRoom&);
	YukigassenRoom& operator=(const YukigassenRoom&);


};

#endif
