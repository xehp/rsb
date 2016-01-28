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

class AvatarPlayerReferences: public MirrorContainer {
public:
	AvatarPlayerReferences();
	virtual ~AvatarPlayerReferences();

	virtual void readSelf(WordReader *wordString);
	//virtual void dataChanged();

	virtual void regNamedObject(MirrorBase *addedObject);
	virtual const char* getTypeName() const {return "AvatarPlayerReferences";};

	int avatarId;
	bool isActive;
	int teamIndex;
	int fragScore;
	int lossScore;

	RsbRoundBuffer *rsbRoundBuffer;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	AvatarPlayerReferences(const AvatarPlayerReferences&);
	AvatarPlayerReferences& operator=(const AvatarPlayerReferences&);
};

#endif /* AVATARPLAYERREFERENCES_H_ */
