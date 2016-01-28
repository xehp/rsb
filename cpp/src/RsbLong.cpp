/*
RsbLong.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


#include "WordReader.h"

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



