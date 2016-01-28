/*
RsbLong.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef RSBINVITEM_H_
#define RSBINVITEM_H_

#include "RsbString.h"

// These objects also have direction and velocity so the name should perhaps be D3ObjWithPOV?

class RsbInvItem: public RsbString
{
public:
	RsbInvItem();
	virtual ~RsbInvItem();

	virtual void renderSelf() {;};

	virtual void readSelf(WordReader *wordString);

	long long getNumberOf() const {return numberOf;};

	virtual const char* getTypeName() const {return "RsbInvItem";};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	RsbInvItem(const RsbInvItem&);
	RsbInvItem& operator=(const RsbInvItem&);


	long long numberOf;
};

#endif
