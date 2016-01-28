/*
RoboGameWorldRoot.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef RoboGameWorldRoot_H
#define RoboGameWorldRoot_H

#include "RoboGameRoom.h"


class RoboGameWorldRoot: public RoboGameRoom
{
public:
	RoboGameWorldRoot();

	virtual ~RoboGameWorldRoot();

	virtual void renderAll();

	//virtual void clear();

	virtual void readSelf(WordReader *wr);



	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	RoboGameWorldRoot(const RoboGameWorldRoot&);
	RoboGameWorldRoot& operator=(const RoboGameWorldRoot&);


};

#endif
