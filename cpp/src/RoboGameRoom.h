/*
RoboGameRoom.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef RoboGameRoom_H
#define RoboGameRoom_H

#include "D3ObjWithPos.h"


class RoboGameRoom: public D3ObjWithPos
{
public:
	RoboGameRoom();
	virtual ~RoboGameRoom();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);



	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	RoboGameRoom(const RoboGameRoom&);
	RoboGameRoom& operator=(const RoboGameRoom&);


	D3Vector roomSize;
};

#endif
