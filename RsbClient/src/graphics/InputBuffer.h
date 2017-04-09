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
class DrawText;

enum InputMode
{
	InputModeFirstPerson=0,
	InputModeEnteringTextString=1,
	InputModeEnteringButtonOrMenu=2,
	InputModeEnteringPhysicalKey=3
};


class InputBuffer: public MirrorContainer
{
public:

	InputBuffer(DrawText *drawText, float centerX, float centerY, float width, float height);
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
	void MoveCursorBack();
	void MoveCursorForward();
	void MoveCursorToEnd();
	float calculateStep();
	const wchar_t* getText() const {return inputBuffer;};

	DrawText *drawText;
	InputMode inputMode;
	wchar_t inputBuffer[256]; // When in InputModeEnteringString the string is stored here.
	unsigned int inputBufferLen; // This tells how many character we have in inputBuffer
	const char *inputBufferPrefix;
	unsigned int inputBufferPos; // This tells where in inputBuffer next character should be written

	Text2DBox *textInputBox;

	wchar_t prevInputBuffer[256]; // Previous string is here


	/**
	 *  xPos/yPos this is where the center of the text shall be placed on the screen (actually where within its window on screen).
	 *  It is given in open GL coordinates so (-1,-1) is lower left and (1,1) is upper right.
	 */
	const float xPos;
	const float yPos;

	/**
	 *  height/width is how big the text window is on screen, in open GL style (that is 2.0 would take up the entire screen).
	 */
	const float width;
	const float height;

};


#endif /* INPUTBUFFER_H_ */
