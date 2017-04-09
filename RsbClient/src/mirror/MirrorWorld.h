/*
MirrorWorld.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef MirrorWorld_H
#define MirrorWorld_H

#include "D3Vector.h"
#include "MirrorIdList.h"

// The world object is special in that for a game there shall only be one world object and it must be the root object in that game.
// TODO Can we (should we) merge MirrorDb and MirrorWorld by letting MirrorWorld extend MirrorDb instead.

class RsbLong;

class MirrorWorld: public MirrorIdList
{
public:
	MirrorWorld();
	virtual ~MirrorWorld();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual void regNamedChildObject(MirrorBase* childObj);


	//virtual long long getGameTime() const {return gameTime->getValue();};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	MirrorWorld(const MirrorWorld&);
	MirrorWorld& operator=(const MirrorWorld&);

	RsbLong* gameTime;
};

#endif
