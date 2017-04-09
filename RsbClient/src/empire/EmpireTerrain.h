/*
EmpireTerrain.h

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/

#ifndef SRC_EmpireTerrain_H_
#define SRC_EmpireTerrain_H_

//#include <cstddef>
//#include <inttypes.h> // wanted to use int64_t but got some sort of ambiguous error from eclipse.
#include <string>
#include "MirrorContainer.h"


class EmpireTerrain: public MirrorContainer
{
public:
	EmpireTerrain();
	virtual ~EmpireTerrain();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireTerrain";};


	int sizeX;
	int sizeY;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireTerrain(const EmpireTerrain&);
	EmpireTerrain& operator=(const EmpireTerrain&);


};

#endif
