/*
MirrorWorld.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/


#include <string.h>
#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "RsbLong.h"
#include "MirrorWorld.h"


MirrorWorld::MirrorWorld() :
	MirrorIdList(),
	gameTime(NULL)
{



}

MirrorWorld::~MirrorWorld()
{}


void MirrorWorld::readSelf(WordReader *wr)
{
	MirrorIdList::readSelf(wr);

	// See: DbRoot.java
	/*rootUpdateCounter=*/wr->readInt();

	// See: WorldBase.java
	/*createdBy =*/wr->readString();
	/*creationTime=*/wr->readLong();
	/*gameTime=*/wr->readLong();
	/*ops=*/wr->readString();
	/*gamePassword=*/wr->readString();
	/*bannedPlayers=*/wr->readString();
	/*customWorldScript=*/wr->readString();
	/*customAvatarScript_no_longer_used=*/wr->readString();
	/*customScriptPath=*/wr->readString();

}

#ifndef __EMSCRIPTEN__
void MirrorWorld::writeSelf(WordWriter *ww)
{
	MirrorIdList::writeSelf(ww);

	// See: DbRoot.java
	ww->writeInt(0);/*rootUpdateCounter*/

	// See: WorldBase.java
	ww->writeString("");/*createdBy*/
	ww->writeLong(0);/*creationTime*/
	ww->writeLong(0);/*gameTime*/
	ww->writeString("");/*ops*/
	ww->writeString("");/*gamePassword*/
	ww->writeString("");/*bannedPlayers*/
	ww->writeString("");/*customWorldScript*/
	ww->writeString("");/*customAvatarScript_no_longer_used*/
	ww->writeString("");/*customScriptPath*/
}
#endif

void MirrorWorld::renderAll()
{
	MirrorIdList::renderAll();
}


void MirrorWorld::regNamedChildObject(MirrorBase* childObj)
{
	if (strcmp(childObj->getName(), "_gameTime")==0)
	{
		gameTime=dynamic_cast<RsbLong*>(childObj);
	}
}
