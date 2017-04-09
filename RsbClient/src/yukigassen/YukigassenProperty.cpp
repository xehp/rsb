/*
 * YukigassenProperty.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: May 7, 2015
 *      Author: henrik
 */

#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "YukigassenRoom.h"
#include "YukigassenProperty.h"

YukigassenProperty::YukigassenProperty() : YukigassenRoom(), ownerId(0) {

}

YukigassenProperty::~YukigassenProperty() {
}



void YukigassenProperty::readSelf(WordReader *wr)
{
	YukigassenRoom::readSelf(wr);

	ownerId = wr->readInt();
}


#ifndef __EMSCRIPTEN__
void YukigassenProperty::writeSelf(WordWriter *ww)
{
	YukigassenRoom::writeSelf(ww);

	//allowedMove.readSelf(wr);
	ww->writeInt(ownerId);
}
#endif
