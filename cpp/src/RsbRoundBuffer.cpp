/*
RsbRoundBuffer.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include <stdlib.h>
#include <string.h>

#include "WordReader.h"

#include "RsbRoundBuffer.h"


RsbRoundBuffer::RsbRoundBuffer() :
	MirrorContainer(),
    head(0),
    tail(0),
	msgCount(0),
	maxObjects(256)
{

}

RsbRoundBuffer::~RsbRoundBuffer()
{
}


void RsbRoundBuffer::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	head=wr->readInt();
	tail=wr->readInt();
	msgCount=wr->readInt();
	maxObjects=wr->readInt();

}



int RsbRoundBuffer::incAndWrap(int i)
{
	if (++i==maxObjects*2)
	{
		i=0;
	}
	return i;
}
