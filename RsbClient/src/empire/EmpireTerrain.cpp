/*
EmpireTerrain.cpp

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
#include "EmpireTerrain.h"


EmpireTerrain::EmpireTerrain() :
MirrorContainer(),
sizeX(0),
sizeY(0)
{



}

EmpireTerrain::~EmpireTerrain()
{}


void EmpireTerrain::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	sizeX=wr->readInt();
	sizeY=wr->readInt();

}

#ifndef __EMSCRIPTEN__
void EmpireTerrain::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);

	ww->writeInt(sizeX);
	ww->writeInt(sizeY);

}
#endif


void EmpireTerrain::renderAll()
{
	MirrorContainer::renderAll();

}
