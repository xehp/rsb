/*
 * RsbRoundBuffer.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 26, 2015
 *      Author: henrik
 */

#ifndef RSBROUNDBUFFER_H_
#define RSBROUNDBUFFER_H_

#include "MirrorContainer.h"

class WordReader;

class RsbRoundBuffer: public MirrorContainer {
public:
	RsbRoundBuffer();
	virtual ~RsbRoundBuffer();

	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif

	virtual const char* getTypeName() const {return "RsbRoundBuffer";};

	int incAndWrap(int i);


    int head; // This is the first (oldest) message in the queue
    int tail; // This is where messages are added to the queue
	int msgCount; // Total number of messages ever added?
	int maxObjects; // max number of messages stored in the round buffer, max index is: maxObjects*2-1


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	RsbRoundBuffer(const RsbRoundBuffer&);
	RsbRoundBuffer& operator=(const RsbRoundBuffer&);

};

#endif /* RSBROUNDBUFFER_H_ */
