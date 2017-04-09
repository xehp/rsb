/*
EmpireActiveObject.cpp

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
#include "EmpireActiveObject.h"


EmpireActiveObject::EmpireActiveObject() :
EmpireUnitOrSector(),
localTime(0)
{



}

EmpireActiveObject::~EmpireActiveObject()
{}


void EmpireActiveObject::readSelf(WordReader *wr)
{
	EmpireUnitOrSector::readSelf(wr);

	localTime=wr->readLong();

}

#ifndef __EMSCRIPTEN__
void EmpireActiveObject::writeSelf(WordWriter *ww)
{
	EmpireUnitOrSector::writeSelf(ww);

	ww->writeLong(localTime);

}
#endif


void EmpireActiveObject::renderAll()
{
	EmpireUnitOrSector::renderAll();

}
