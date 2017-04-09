/*
 * D3Int.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: henrik
 *
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 */

#include "WordReader.h"
#include "WordWriter.h"


#include "D3Int.h"

D3Int::D3Int() : x(0), y(0), z(0) {
}

D3Int::~D3Int() {
}



void D3Int::readSelf(WordReader *wr)
{
    if (wr->equalsIgnoreCase("-"))
    {
    	wr->readToken();
    	x=0;
    	y=0;
    	z=0;
    }
    else
    {
		x=wr->readInt();
		y=wr->readInt();
		z=wr->readInt();
    }
}

void D3Int::writeSelf(WordWriter *ww)
{
	ww->writeInt(x);
	ww->writeInt(y);
	ww->writeInt(z);
}
