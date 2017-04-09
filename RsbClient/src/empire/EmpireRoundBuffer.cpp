/*
EmpireRoundBuffer.cpp

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/


#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "EmpireRoundBuffer.h"

/*
EmpireRoundBuffer::EmpireRoundBuffer() :
MirrorContainer(),
head(0),
tail(0),
msgCount(0),
maxObjects(64)
{



}

EmpireRoundBuffer::~EmpireRoundBuffer()
{}


void EmpireRoundBuffer::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	head=wr->readInt();
	tail=wr->readInt();
	msgCount=wr->readInt();
	maxObjects=wr->readInt();

}

#ifndef __EMSCRIPTEN__
void EmpireRoundBuffer::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);

	ww->writeInt(head);
	ww->writeInt(tail);
	ww->writeInt(msgCount);
	ww->writeInt(maxObjects);

}
#endif


void EmpireRoundBuffer::renderAll()
{
	MirrorContainer::renderAll();

}
*/

EmpireRoundBuffer::EmpireRoundBuffer() :
RsbRoundBuffer()
{



}

EmpireRoundBuffer::~EmpireRoundBuffer()
{}


