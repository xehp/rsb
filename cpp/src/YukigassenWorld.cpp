/*
YukigassenWorld.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


#include "WordReader.h"
#include "YukigassenWorld.h"


YukigassenWorld::YukigassenWorld() :
	MirrorWorld(),
	turn(0)
	//,startTime(0)
{



}

YukigassenWorld::~YukigassenWorld()
{}


void YukigassenWorld::readSelf(WordReader *wr)
{
	MirrorWorld::readSelf(wr);

	turn = wr->readInt();
	//startTime= wr->readLong();

}



void YukigassenWorld::renderAll()
{
	MirrorWorld::renderAll();

}
