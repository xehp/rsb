/*
 * ActivePlayerList.h
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: May 13, 2015
 *      Author: henrik
 */

#ifndef ACTIVEPLAYERLIST_H_
#define ACTIVEPLAYERLIST_H_

#include "MirrorContainer.h"

class ActivePlayerList: public MirrorContainer {
public:
	ActivePlayerList();
	virtual ~ActivePlayerList();
	virtual const char* getTypeName() const {return "ActivePlayerList";};


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	ActivePlayerList(const ActivePlayerList&);
	ActivePlayerList& operator=(const ActivePlayerList&);

};

#endif /* ACTIVEPLAYERLIST_H_ */
