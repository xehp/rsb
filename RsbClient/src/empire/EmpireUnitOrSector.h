/*
EmpireUnitOrSector.h

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/

#ifndef SRC_EmpireUnitOrSector_H_
#define SRC_EmpireUnitOrSector_H_

//#include <cstddef>
//#include <inttypes.h> // wanted to use int64_t but got some sort of ambiguous error from eclipse.
#include <string>
#include "MirrorContainer.h"


class EmpireUnitOrSector: public MirrorContainer
{
public:
	EmpireUnitOrSector();
	virtual ~EmpireUnitOrSector();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireUnitOrSector";};

	int owner;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireUnitOrSector(const EmpireUnitOrSector&);
	EmpireUnitOrSector& operator=(const EmpireUnitOrSector&);


};

#endif
