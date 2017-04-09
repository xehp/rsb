/*
EmpireStatesList.cpp

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
#include "EmpireStatesList.h"


EmpireStatesList::EmpireStatesList() :
MirrorContainer()
{



}

EmpireStatesList::~EmpireStatesList()
{}


void EmpireStatesList::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);


}

#ifndef __EMSCRIPTEN__
void EmpireStatesList::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);

}
#endif


void EmpireStatesList::renderAll()
{
	MirrorContainer::renderAll();

}
