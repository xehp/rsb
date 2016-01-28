/*
RoboGameWorld.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef RoboGameWorld_H
#define RoboGameWorld_H

#include "MirrorWorld.h"


class RoboGameWorld: public MirrorWorld
{
public:
	RoboGameWorld();
	virtual ~RoboGameWorld();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);



	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	RoboGameWorld(const RoboGameWorld&);
	RoboGameWorld& operator=(const RoboGameWorld&);


	int turn;
	long long startTime;
	//long long typeOfWorld;

};

#endif
