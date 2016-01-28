/*
YukigassenWorld.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef YukigassenWorld_H
#define YukigassenWorld_H

#include "MirrorWorld.h"


class YukigassenWorld: public MirrorWorld
{
public:
	YukigassenWorld();
	virtual ~YukigassenWorld();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);

	virtual const char* getTypeName() const {return "YukigassenWorld";};


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenWorld(const YukigassenWorld&);
	YukigassenWorld& operator=(const YukigassenWorld&);


	int turn;
};

#endif
