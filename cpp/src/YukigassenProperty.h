/*
 * YukigassenProperty.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: May 7, 2015
 *      Author: henrik
 */

#ifndef YUKIGASSENPROPERTY_H_
#define YUKIGASSENPROPERTY_H_

#include "YukigassenRoom.h"

class YukigassenProperty : public YukigassenRoom {
public:
	YukigassenProperty();
	virtual ~YukigassenProperty();

	virtual void readSelf(WordReader *wr);
	virtual const char* getTypeName() const {return "YukigassenProperty";};

	int ownerId;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenProperty(const YukigassenProperty&);
	YukigassenProperty& operator=(const YukigassenProperty&);

};

#endif /* YUKIGASSENPROPERTY_H_ */

