/*
RsbLong.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef RsbLong_H
#define RsbLong_H

#include "MirrorContainer.h"

// These objects also have direction and velocity so the name should perhaps be D3ObjWithPOV?

class RsbLong: public MirrorContainer
{
public:
	RsbLong();
	virtual ~RsbLong();

	virtual void renderSelf() {;};

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	long long getValue() const {return value;};

	virtual const char* getTypeName() const {return "RsbLong";};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	RsbLong(const RsbLong&);
	RsbLong& operator=(const RsbLong&);


	long long value;
};

#endif
