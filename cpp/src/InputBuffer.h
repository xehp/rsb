/*
 * inputBuffer.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 30, 2015
 *      Author: henrik
 */

#ifndef INPUTBUFFER_H_
#define INPUTBUFFER_H_

#include "MirrorContainer.h"

class Text2DBox;


enum InputMode
{
	InputModeIdle=0,
	InputModeEnteringString=1,
	InputModeEnteringButton=2,
	InputModeEnteringKey=3
};


class InputBuffer: public MirrorContainer
{
public:

	InputBuffer();
	virtual ~InputBuffer();

	virtual void unlinkAllChildObjects();
	virtual void clearText();

	void inputEnterStringEntryMode();
	void inputEnterButtonEntryMode();
	void inputEnterKeyEntryMode();
	void AddChar(int ch);
	void inputBufferClear();
	void DelChar();
	void DoneNowSend();
	InputMode GetMode();
	void SetPrefix(const char* str);
	void setUp();
	void getPrev();

	InputMode inputMode;
	char inputBuffer[256]; // When in InputModeEnteringString the string is stored here.
	unsigned int inputBufferIndex; // This tells how many character we have in inputBuffer
	const char *inputBufferPrefix;

	Text2DBox *textInputBox; // TODO: move this out of the TextPage class.

	char prevInputBuffer[256]; // Previous string is here

};


#endif /* INPUTBUFFER_H_ */
