/*
 * AvatarPlayerReferences.cpp
 *
 *  Created on: May 13, 2015
 *      Author: henrik
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 */
#include <stdio.h>
#include <string.h>

#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "RsbRoundBuffer.h"
//#include "YukigassenAvatar.h"
#include "GameUser.h"

GameUser::GameUser() :
	MirrorContainer(),
	avatarId(-1),
	isActive(false),
	teamIndex(0),
	fragScore(0),
	lossScore(0),
	rsbRoundBuffer(NULL)
{
}


GameUser::~GameUser() {
}


void GameUser::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);
	avatarId=wr->readInt();
	isActive=wr->readBoolean();
	teamIndex=wr->readInt();
	fragScore=wr->readInt();
	lossScore=wr->readInt();
}

#ifndef __EMSCRIPTEN__
void GameUser::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);
	ww->writeInt(avatarId);
	ww->writeInt(isActive?1:0);
	ww->writeInt(teamIndex);
	ww->writeInt(fragScore);
	ww->writeInt(lossScore);
}
#endif

#if 0
void GameUser::dataChanged()
{
	MirrorContainer::dataChanged();

	/*YukigassenAvatar *ya=dynamic_cast<YukigassenAvatar *>(mirrorDb->getObjectById(avatarId));
	if (ya!=NULL)
	{
		ya->teamIndex = teamIndex;
	}
	else
	{
		// It is a problem if client get this object before the YukigassenAvatar object. If so then the teamIndex can not be set this way. Perhaps we shall create the avatar object here?
		printf("AvatarPlayerReferences no YukigassenAvatar\n");
	}*/
}
#endif

void GameUser::regNamedChildObject(MirrorBase *addedObject)
{
	RsbRoundBuffer *o=dynamic_cast<RsbRoundBuffer *>(addedObject);

	if ((o!=NULL) && (rsbRoundBuffer==NULL) && (strcmp(addedObject->getName(), "_rrb")==0))
	{
		rsbRoundBuffer = o;
	}
	else
	{
		// Perhaps super class knows what to do with this one?
		MirrorContainer::regNamedChildObject(addedObject);
	}

}
