/*
EmpireState.cpp

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
#include "EmpireState.h"


EmpireState::EmpireState() :
EmpireActiveObject(),
moneyEarned(0),
moneySpent(0),
headOfState(),
allyList(),
homeSectorId(0),
StateMode(0),
coRuler(),
hostileToState(),
stateMotto(),
workforceAvailability(0),
workforceInUse(0)
{



}

EmpireState::~EmpireState()
{}


void EmpireState::readSelf(WordReader *wr)
{
	EmpireActiveObject::readSelf(wr);

	moneyEarned=wr->readLong();
	moneySpent=wr->readLong();
	headOfState=wr->readString();
	allyList=wr->readString(); // new BigBitMap(wr->readString());
	homeSectorId=wr->readInt();
	StateMode=wr->readLong();
	coRuler=wr->readString();
	hostileToState=wr->readString(); //new BigBitMap(wr->readString());
	stateMotto=wr->readString();
	workforceAvailability=wr->readLong();;
	workforceInUse=wr->readLong();;

}

#ifndef __EMSCRIPTEN__
void EmpireState::writeSelf(WordWriter *ww)
{
	EmpireActiveObject::writeSelf(ww);

	ww->writeLong(moneyEarned);
	ww->writeLong(moneySpent);
	ww->writeString(headOfState);
	ww->writeString(allyList); // allyList.toString()
	ww->writeInt(homeSectorId);
	ww->writeLong(StateMode);
	ww->writeString(coRuler);
	ww->writeString(hostileToState); // hostileToState.toString()
	ww->writeString(stateMotto);
	ww->writeLong(workforceAvailability);
	ww->writeLong(workforceInUse);

}
#endif


void EmpireState::renderAll()
{
	EmpireActiveObject::renderAll();

}
