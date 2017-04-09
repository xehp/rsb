/*
 * YukigassenEntity.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 17, 2015
 *      Author: henrik
 */

#ifndef YUKIGASSENENTITY_H_
#define YUKIGASSENENTITY_H_

#include "YukigassenProperty.h"

class YukigassenEntity : public YukigassenProperty {
public:
	YukigassenEntity();
	virtual ~YukigassenEntity();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	int entityColor;
	int textureIndex;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenEntity(const YukigassenEntity&);
	YukigassenEntity& operator=(const YukigassenEntity&);

};

#endif /* YUKIGASSENENTITY_H_ */
