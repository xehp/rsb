/*
EmpireRoundBuffer.h

www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2017 by Henrik Bjorkman www.eit.se

*/

// TODO there is also a class RsbRoundBuffer, can we use that here?


#ifndef SRC_EmpireRoundBuffer_H_
#define SRC_EmpireRoundBuffer_H_

//#include <cstddef>
//#include <inttypes.h> // wanted to use int64_t but got some sort of ambiguous error from eclipse.
#include <string>
#include "MirrorContainer.h"
#include "RsbRoundBuffer.h"

/*
class EmpireRoundBuffer: public MirrorContainer
{
public:
	EmpireRoundBuffer();
	virtual ~EmpireRoundBuffer();

	virtual void renderAll();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "EmpireRoundBuffer";};

    int head; // This is the first (oldest) message in the queue
    int tail; // This is where messages are added to the queue
	int msgCount;
	int maxObjects;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireRoundBuffer(const EmpireRoundBuffer&);
	EmpireRoundBuffer& operator=(const EmpireRoundBuffer&);


};
*/

class EmpireRoundBuffer: public RsbRoundBuffer
{
public:
	EmpireRoundBuffer();
	virtual ~EmpireRoundBuffer();

	virtual const char* getTypeName() const {return "EmpireRoundBuffer";};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	EmpireRoundBuffer(const EmpireRoundBuffer&);
	EmpireRoundBuffer& operator=(const EmpireRoundBuffer&);

};


#endif
