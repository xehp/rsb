/*
YukigassenInventory.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2016-01-03 by Henrik Bjorkman www.eit.se

*/

#ifndef YUKIGASSENINVENTORY_H_
#define YUKIGASSENINVENTORY_H_

#include "MirrorContainer.h"

// These objects also have direction and velocity so the name should perhaps be D3ObjWithPOV?

class YukigassenInventory: public MirrorContainer
{
public:
	YukigassenInventory();
	virtual ~YukigassenInventory();

	virtual const char* getTypeName() const {return "YukigassenInventory";};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	YukigassenInventory(const YukigassenInventory&);
	YukigassenInventory& operator=(const YukigassenInventory&);
};


#endif /* YUKIGASSENINVENTORY_H_ */
