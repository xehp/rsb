/*
RoboGameWorld.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


#include "WordReader.h"
#include "RoboGameWorld.h"


RoboGameWorld::RoboGameWorld() :
	MirrorWorld(),
	turn(0),
	startTime(0)
{



}

RoboGameWorld::~RoboGameWorld()
{}


void RoboGameWorld::readSelf(WordReader *wr)
{
	MirrorWorld::readSelf(wr);

	turn = wr->readInt();
	startTime= wr->readLong();

}



void RoboGameWorld::renderAll()
{
	MirrorWorld::renderAll();

}
