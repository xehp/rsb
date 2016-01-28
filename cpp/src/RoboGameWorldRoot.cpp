/*
RoboGameWorldRoot.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/



#include "RoboGameWorldRoot.h"


RoboGameWorldRoot::RoboGameWorldRoot() :
	RoboGameRoom()
{



}


RoboGameWorldRoot::~RoboGameWorldRoot()
{}



void RoboGameWorldRoot::readSelf(WordReader *wr)
{
	RoboGameRoom::readSelf(wr);


}



void RoboGameWorldRoot::renderAll()
{
	MirrorContainer::renderAll();

}
