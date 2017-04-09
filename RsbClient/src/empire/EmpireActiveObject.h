/*
EmpireActiveObject.h

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/

#ifndef SRC_EmpireActiveObject_H_
#define SRC_EmpireActiveObject_H_

#include "EmpireUnitOrSector.h"


class EmpireActiveObject: public EmpireUnitOrSector
{
public:
	EmpireActiveObject();
	virtual ~EmpireActiveObject();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireActiveObject";};

	long long localTime;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireActiveObject(const EmpireActiveObject&);
	EmpireActiveObject& operator=(const EmpireActiveObject&);


};

#endif
