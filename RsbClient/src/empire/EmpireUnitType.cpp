/*
EmpireUnitType.cpp

www->eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www->eit.se/hb)
License: www->eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www->eit.se

*/


#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "EmpireUnitType.h"


EmpireUnitType::EmpireUnitType() :
MirrorContainer(),
//typeName(),
buildMoneyCost(0),
buildCapacityCost(0),
unitIncome(0),
landMoveCost(0),
seaMoveCost(0),
unitMass(0),
landCarryCapacity(0),
seaCarryCapacity(0),
maxHealth(0),
attackProbability(0),
attackStrength(0),
landDefenseMoving(0),
landDefenseStill(0),
seaDefence(0),
personnelSupport(0),
crewNeeded(0)//,
//degradesTo(),
//canTransformInto(),
//transformationMaterial(),
//disassemblesInto(),
//possibleOrders(),
//possibleBuilds(),
//buildMaterialNeeded(),
//requiredTerrain(),
//possibleAssemblies()
{
}

EmpireUnitType::~EmpireUnitType()
{
	// Remember to free all "stringCopy"
/*
	stringCopyFreeW(typeName);
	stringCopyFree(degradesTo);
	stringCopyFree(canTransformInto);
	stringCopyFree(transformationMaterial);
	stringCopyFree(disassemblesInto);
	stringCopyFree(possibleOrders);
	stringCopyFree(possibleBuilds);
	stringCopyFree(buildMaterialNeeded);
	stringCopyFree(requiredTerrain);
	stringCopyFree(possibleAssemblies);*/
	// Or if we use std::string then delete is not needed?
}


void EmpireUnitType::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	typeName = wr->readWord();
	buildMoneyCost=wr->readInt();
	buildCapacityCost=wr->readInt();
	unitIncome=wr->readInt();
	landMoveCost=wr->readInt();
	seaMoveCost=wr->readInt();
	unitMass=wr->readInt();
	landCarryCapacity=wr->readInt();
	seaCarryCapacity=wr->readInt();
	maxHealth=wr->readInt();
	attackProbability=wr->readInt();
	attackStrength=wr->readInt();
	landDefenseMoving=wr->readInt();
	landDefenseStill=wr->readInt();
	seaDefence=wr->readInt();
	personnelSupport=wr->readInt();
	crewNeeded=wr->readInt();
	degradesTo=wr->readString();
	canTransformInto=wr->readString();
	transformationMaterial=wr->readString();
	disassemblesInto=wr->readString();
	possibleOrders=wr->readString();
	possibleBuilds=wr->readString();
	buildMaterialNeeded=wr->readString();
	requiredTerrain=wr->readString();
	possibleAssemblies=wr->readString();

}

#ifndef __EMSCRIPTEN__
void EmpireUnitType::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);

	ww->writeWord(typeName);
	ww->writeInt(buildMoneyCost);
	ww->writeInt(buildCapacityCost);
	ww->writeInt(unitIncome);
	ww->writeInt(landMoveCost);
	ww->writeInt(seaMoveCost);
	ww->writeInt(unitMass);
	ww->writeInt(landCarryCapacity);
	ww->writeInt(seaCarryCapacity);
	ww->writeInt(maxHealth);
	ww->writeInt(attackProbability);
	ww->writeInt(attackStrength);
	ww->writeInt(landDefenseMoving);
	ww->writeInt(landDefenseStill);
	ww->writeInt(seaDefence);
	ww->writeInt(personnelSupport);
	ww->writeInt(crewNeeded);
	ww->writeString(degradesTo);
	ww->writeString(canTransformInto);
	ww->writeString(transformationMaterial);
	ww->writeString(disassemblesInto);
	ww->writeString(possibleOrders);
	ww->writeString(possibleBuilds);
	ww->writeString(buildMaterialNeeded);
	ww->writeString(requiredTerrain);
	ww->writeString(possibleAssemblies);

}
#endif


void EmpireUnitType::renderAll()
{
	MirrorContainer::renderAll();

}
