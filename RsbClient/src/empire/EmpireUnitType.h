/*
EmpireUnitType.h

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/

#ifndef SRC_EmpireUnitType_H_
#define SRC_EmpireUnitType_H_

//#include <cstddef>
//#include <inttypes.h> // wanted to use int64_t but got some sort of ambiguous error from eclipse.
#include <string>
#include "MirrorContainer.h"


class EmpireUnitType: public MirrorContainer
{
public:
	EmpireUnitType();
	virtual ~EmpireUnitType();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireUnitType";};



	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireUnitType(const EmpireUnitType&);
	EmpireUnitType& operator=(const EmpireUnitType&);


	std::string typeName;
	int buildMoneyCost;
	int buildCapacityCost;
	int unitIncome;
	int landMoveCost;
	int seaMoveCost;
	int unitMass;
	int landCarryCapacity;
	int seaCarryCapacity;
	int maxHealth;
	int attackProbability;
	int attackStrength;
	int landDefenseMoving;
	int landDefenseStill;
	int seaDefence;
	int personnelSupport;
	int crewNeeded;
	std::string degradesTo;
	std::string canTransformInto;
	std::string transformationMaterial;
	std::string disassemblesInto;
	std::string possibleOrders;
	std::string possibleBuilds;
	std::string buildMaterialNeeded;
	std::string requiredTerrain;
	std::string possibleAssemblies;


};

#endif
