/*
RsbLong.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif

#include "RsbLong.h"


RsbLong::RsbLong() :
	MirrorContainer(),
	value(0)
{

}

RsbLong::~RsbLong()
{}


void RsbLong::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);
	value = wr->readLong();
}

#ifndef __EMSCRIPTEN__
void RsbLong::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);
	ww->writeLong(value);
}
#endif


