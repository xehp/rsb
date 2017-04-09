/*
EmpireSector.cpp

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
#include "EmpireSector.h"


EmpireSector::EmpireSector() :
EmpireUnitOrSector(),
terrain(0)
{



}

EmpireSector::~EmpireSector()
{}


void EmpireSector::readSelf(WordReader *wr)
{
	EmpireUnitOrSector::readSelf(wr);

	terrain=wr->readInt();

}

#ifndef __EMSCRIPTEN__
void EmpireSector::writeSelf(WordWriter *ww)
{
	EmpireUnitOrSector::writeSelf(ww);

	ww->writeInt(terrain);

}
#endif


void EmpireSector::renderAll()
{
	EmpireUnitOrSector::renderAll();

}
