/*
 * TextPageScroll.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Jul 20, 2015
 *      Author: drekkar
 */

#ifndef TEXTPAGESCROLL_H_
#define TEXTPAGESCROLL_H_

#include "TextPage.h"

#define TEXT_BUFFER_SIZE 1024

class Color2DBox;
class Color2DFrame;

class TextRoundBuffer
{
public:
	TextRoundBuffer();
	~TextRoundBuffer();

	int incAndWrap(int i);
	void addText(const char *displayStr, const char* replyStr);
	void clearText();
	const char* getTextString(unsigned int index);
	const char* getTextName(unsigned int index);
	int getSize() {return size;};

private:
	int head; // this index tells wish line is the oldest in our buffer
	int tail; // this index tells where we shall add next line
	int size; // how many lines are stored

	char *textString[TEXT_BUFFER_SIZE]; // This is the text buffer alias display string
	char *textNames[TEXT_BUFFER_SIZE]; // alias reply string


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	TextRoundBuffer(const TextRoundBuffer&);
	TextRoundBuffer& operator=(const TextRoundBuffer&);
};


class TextPageScroll: public TextPage {
public:
	TextPageScroll();
	virtual ~TextPageScroll();

	virtual void clearText();
	virtual void addText(const char *displayStr, const char* replyStr=NULL); // add text at bottom
	virtual void setUp();
	virtual void scrollUp();
	virtual void scrollDown();
	virtual void setScrollBar();
	virtual void unlinkAllChildObjects();
	virtual void createScrollBar();


	int scrollPos;   // First line (of the lines in textRoundBuffer) to be shown, if zero the line at head is to be in top of page


	Color2DBox *color2DBox;
	Color2DFrame *color2DBox2;

	TextRoundBuffer textRoundBuffer;



	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	TextPageScroll(const TextPageScroll&);
	TextPageScroll& operator=(const TextPageScroll&);

};

extern TextPageScroll *textMessagesPage;
extern TextPageScroll *textStatisticsPage;
extern TextPageScroll *textInventoryPage;
extern TextPageScroll *textFocusPage;

#endif /* TEXTPAGESCROLL_H_ */
