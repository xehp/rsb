/*
RoboGameRoom.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/



#include "RoboGameRoom.h"


RoboGameRoom::RoboGameRoom() :
	D3ObjWithPos(),
	roomSize()
{


}


RoboGameRoom::~RoboGameRoom()
{
}



void RoboGameRoom::readSelf(WordReader *wr)
{
	D3ObjWithPos::readSelf(wr);

	roomSize.readSelf(wr);
}


void RoboGameRoom::renderAll()
{
	MirrorContainer::renderAll();

}
