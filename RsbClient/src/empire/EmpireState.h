/*
EmpireState.h

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/

#ifndef SRC_EmpireState_H_
#define SRC_EmpireState_H_

//#include <cstddef>
//#include <inttypes.h> // wanted to use int64_t but got some sort of ambiguous error from eclipse.
#include <string>
#include "EmpireActiveObject.h"


class EmpireState: public EmpireActiveObject
{
public:
	EmpireState();
	virtual ~EmpireState();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireState";};

	long long moneyEarned;
	long long moneySpent;
	std::string headOfState;
	std::string allyList; // TODO BigBitMap
	int homeSectorId;
	long long StateMode;
	std::string coRuler;
	std::string hostileToState; // TODO BigBitMap
	std::string stateMotto;
	long long workforceAvailability;
	long long workforceInUse;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireState(const EmpireState&);
	EmpireState& operator=(const EmpireState&);


};

#endif
