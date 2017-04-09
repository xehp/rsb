/*
EmpireSector.h

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/

#ifndef SRC_EmpireSector_H_
#define SRC_EmpireSector_H_

//#include <cstddef>
//#include <inttypes.h> // wanted to use int64_t but got some sort of ambiguous error from eclipse.
#include <string>
#include "MirrorContainer.h"
#include "EmpireUnitOrSector.h"


class EmpireSector: public EmpireUnitOrSector
{
public:
	EmpireSector();
	virtual ~EmpireSector();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireSector";};

	int terrain;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireSector(const EmpireSector&);
	EmpireSector& operator=(const EmpireSector&);


};

#endif
