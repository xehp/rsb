/*
 * YukigassenEntity.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: Apr 17, 2015
 *      Author: henrik
 */

#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "YukigassenRoom.h"
#include "YukigassenEntity.h"

YukigassenEntity::YukigassenEntity() : YukigassenProperty(), entityColor(0), textureIndex(0) {
}

YukigassenEntity::~YukigassenEntity() {
}

void YukigassenEntity::readSelf(WordReader *wr)
{
	YukigassenProperty::readSelf(wr);

	textureIndex = wr->readInt();
	entityColor = wr->readInt();

}


#ifndef __EMSCRIPTEN__
void YukigassenEntity::writeSelf(WordWriter *ww)
{
	YukigassenProperty::writeSelf(ww);

   	ww->writeInt(textureIndex);
   	ww->writeInt(entityColor);
}
#endif