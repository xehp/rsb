/*
RsbLong.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef RSBSTRING_H_
#define RSBSTRING_H_

#include "MirrorContainer.h"

// These objects also have direction and velocity so the name should perhaps be D3ObjWithPOV?

class RsbString: public MirrorContainer
{
public:
	RsbString();
	virtual ~RsbString();

	virtual void renderSelf() {;};

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	const wchar_t* getValueW() const {return value;};
	void setValueW(const wchar_t*);

	virtual const char* getTypeName() const {return "RsbString";};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	RsbString(const RsbString&);
	RsbString& operator=(const RsbString&);


	wchar_t* value;
};

#endif
