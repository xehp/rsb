/*
EmpireWorld.cpp

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
#include "EmpireWorld.h"


EmpireWorld::EmpireWorld() :
	MirrorWorld(),
	gameTimeMsCounter(0),
	tickState(0),
	gameSpeed(0)
{



}

EmpireWorld::~EmpireWorld()
{}


void EmpireWorld::readSelf(WordReader *wr)
{
	MirrorWorld::readSelf(wr);

	gameTimeMsCounter=wr->readLong();
	tickState=wr->readInt();
	gameSpeed=wr->readInt();

}

#ifndef __EMSCRIPTEN__
void EmpireWorld::writeSelf(WordWriter *ww)
{
	MirrorWorld::writeSelf(ww);

	ww->writeLong(gameTimeMsCounter);
	ww->writeInt(tickState);
	ww->writeInt(gameSpeed);
}
#endif


void EmpireWorld::renderAll()
{
	MirrorWorld::renderAll();

}
