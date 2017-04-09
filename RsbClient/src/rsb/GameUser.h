/*
 * AvatarPlayerReferences.h
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: May 13, 2015
 *      Author: henrik
 */

#ifndef AVATARPLAYERREFERENCES_H_
#define AVATARPLAYERREFERENCES_H_

#include "MirrorContainer.h"

class WordReader;
class RsbRoundBuffer;

class GameUser: public MirrorContainer {
public:
	GameUser();
	virtual ~GameUser();

	virtual void readSelf(WordReader *wordString);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif
	//virtual void dataChanged();

	virtual void regNamedChildObject(MirrorBase *addedObject);
	virtual const char* getTypeName() const {return "GameUser";};

	int avatarId;
	bool isActive;
	int teamIndex;
	int fragScore;
	int lossScore;

	RsbRoundBuffer *rsbRoundBuffer;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	GameUser(const GameUser&);
	GameUser& operator=(const GameUser&);
};

#endif /* AVATARPLAYERREFERENCES_H_ */
