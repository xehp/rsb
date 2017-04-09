/*
YukigassenAvatar.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef YukigassenAvatar_H
#define YukigassenAvatar_H

#include "OrtogonalBox.h"
#include "YukigassenTexturedRoom.h"
#include "RsbLong.h"

class YukigassenTextBillboard;
class WordReader;
//class RsbLong;
class YukigassenInventory;
class TextBillboard;

class YukigassenAvatar: public YukigassenRoom
{
public:
	YukigassenAvatar();
	virtual ~YukigassenAvatar();

	virtual void renderSelf();
	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	//virtual void limitPos(D3Vector &pos);
	virtual void setupInternalRenderingData();
	virtual void renderAll();
	virtual void regNamedChildObject(MirrorBase *addedObject);
	virtual void linkSelf(MirrorBase *parentPtr);

	//OrtogonalBox allowedMove;

	long objectTime;
	int avatarStateMs;
	int avatarAmmo;
	int avatarPlayerReferencesId;
	int serverChangedPositionCounter;

	static const float headSize;
	static const float bodyLength;
	static const float noseToFeetLength;

	virtual const char* getTypeName() const {return "YukigassenAvatar";};

	const YukigassenInventory* getInv() const {return inv;};
	//int teamIndex;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenAvatar(const YukigassenAvatar&);
	YukigassenAvatar& operator=(const YukigassenAvatar&);


	//YukigassenTexturedRoom *head1;
	//YukigassenTexturedRoom *head2;
	//YukigassenTexturedRoom *body;

	TextBillboard *nameTag;

	YukigassenInventory *inv;
};

#endif
