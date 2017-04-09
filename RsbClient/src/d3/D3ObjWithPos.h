/*
D3ObjWithPos.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef D3ObjWithPos_H
#define D3ObjWithPos_H

#include "D3Vector.h"
#include "MirrorContainer.h"
#include "D3PosAndOrientation.h"

// These objects also have direction and velocity so the name should perhaps be D3ObjWithPOV?

class D3ObjWithPos: public MirrorContainer
{
public:
	D3ObjWithPos();
	virtual ~D3ObjWithPos();

	virtual void renderAll();
	//virtual void renderSelf();
	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif
	//virtual D3PosAndOrientation* getPao() {return &pao;};

	D3PosAndOrientation pao;
	D3Vector objectVelocity;

	virtual void translatePaoToGlobal(D3PosAndOrientation &pao) const;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	D3ObjWithPos(const D3ObjWithPos&);
	D3ObjWithPos& operator=(const D3ObjWithPos&);


	//long long gameTimeOfLastUpdate;
	int tickOfLastUpdateMs;
};

#endif
