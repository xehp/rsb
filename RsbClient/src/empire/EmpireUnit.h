/*
EmpireUnit.h

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/

#ifndef SRC_EmpireUnit_H_
#define SRC_EmpireUnit_H_

#include "EmpireActiveObject.h"


class EmpireUnit: public EmpireActiveObject
{
public:
	EmpireUnit();
	virtual ~EmpireUnit();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireUnit";};

	int unitHealth;
	int unitType;
	int attackPoints;
	int movePoints;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireUnit(const EmpireUnit&);
	EmpireUnit& operator=(const EmpireUnit&);


};

#endif
