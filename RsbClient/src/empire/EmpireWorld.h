/*
EmpireWorld.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef EmpireWorld_H
#define EmpireWorld_H

//#include <cstddef>
//#include <inttypes.h> // wanted to use int64_t but got some sort of ambiguous error from eclipse.
#include "MirrorWorld.h"


class EmpireWorld: public MirrorWorld
{
public:
	EmpireWorld();
	virtual ~EmpireWorld();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireWorld";};



	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireWorld(const EmpireWorld&);
	EmpireWorld& operator=(const EmpireWorld&);

	long long gameTimeMsCounter;
	//int64_t gameTimeMsCounter;
	int tickState;
	int gameSpeed;

};

#endif
