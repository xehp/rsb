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
#include "MirrorContainer.h"

// These objects also have direction and velocity so the name should perhaps be D3ObjWithPOV?

class MirrorWorld: public MirrorContainer
{
public:
	MirrorWorld();
	virtual ~MirrorWorld();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);



	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	MirrorWorld(const MirrorWorld&);
	MirrorWorld& operator=(const MirrorWorld&);

};

#endif
