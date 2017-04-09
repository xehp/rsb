/*
EmpireUnitOrSector.cpp

www->eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www->eit.se/hb)
License: www->eit.se/rsb/license

History:
Created Feb 2017 by Henrik Bjorkman www->eit.se

*/


#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "EmpireUnitOrSector.h"


EmpireUnitOrSector::EmpireUnitOrSector() :
MirrorContainer(),
owner(0)
{



}

EmpireUnitOrSector::~EmpireUnitOrSector()
{}


void EmpireUnitOrSector::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	owner=wr->readInt();

}

#ifndef __EMSCRIPTEN__
void EmpireUnitOrSector::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);

	ww->writeInt(owner);

}
#endif


void EmpireUnitOrSector::renderAll()
{
	MirrorContainer::renderAll();

}
