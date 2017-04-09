/*
EmpireUnit.cpp

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
#include "EmpireUnitOrSector.h"
#include "EmpireUnit.h"


EmpireUnit::EmpireUnit() :
EmpireActiveObject(),
unitHealth(0),
unitType(0),
attackPoints(0),
movePoints(0)
{



}

EmpireUnit::~EmpireUnit()
{}


void EmpireUnit::readSelf(WordReader *wr)
{
	EmpireActiveObject::readSelf(wr);

	unitHealth=wr->readInt();
	unitType=wr->readInt();
	attackPoints=wr->readInt();
	movePoints=wr->readInt();

}

#ifndef __EMSCRIPTEN__
void EmpireUnit::writeSelf(WordWriter *ww)
{
	EmpireActiveObject::writeSelf(ww);

	ww->writeInt(unitHealth);
	ww->writeInt(unitType);
	ww->writeInt(attackPoints);
	ww->writeInt(movePoints);

}
#endif


void EmpireUnit::renderAll()
{
	EmpireActiveObject::renderAll();

}
