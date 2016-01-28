/*
 * OrtogonalBox.cpp
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *
 *  Created on: Apr 14, 2015
 *      Author: henrik
 */

#include "WordReader.h"
#include "WordWriter.h"


#include "OrtogonalBox.h"

OrtogonalBox::OrtogonalBox() :
	ux(0.0f),
	lx(0.0f),
	uy(0.0f),
	ly(0.0f),
	uz(0.0f),
	lz(0.0f)
{

}

OrtogonalBox::~OrtogonalBox() {
}





void OrtogonalBox::readSelf(WordReader *wr)
{
	ux = wr->readFloat();
	lx = wr->readFloat();
	uy = wr->readFloat();
	ly = wr->readFloat();
	uz = wr->readFloat();
	lz = wr->readFloat();
}


void OrtogonalBox::writeSelf(WordWriter *ww)
{
	ww->writeFloat(ux);
	ww->writeFloat(lx);
	ww->writeFloat(uy);
	ww->writeFloat(ly);
	ww->writeFloat(uz);
	ww->writeFloat(lz);
}
