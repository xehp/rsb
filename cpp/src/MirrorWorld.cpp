/*
MirrorWorld.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/



#include "WordReader.h"

#include "MirrorWorld.h"


MirrorWorld::MirrorWorld() :
	MirrorContainer()
{



}

MirrorWorld::~MirrorWorld()
{}


void MirrorWorld::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	// See: DbRoot.java
	/*rootUpdateCounter=*/wr->readInt();

	// See: WorldBase.java
	/*createdBy =*/wr->readString();
	/*creationTime=*/wr->readLong();
	/*gameTime=*/wr->readLong();
	/*ops=*/wr->readString();
	/*gamePassword=*/wr->readString();
	/*bannedPlayers=*/wr->readString();


}



void MirrorWorld::renderAll()
{
	MirrorContainer::renderAll();
}
