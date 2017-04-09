/*
 * TextPage.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 */

#ifndef TEXTPAGE_H_
#define TEXTPAGE_H_

#include "MirrorContainer.h"

class Text2DBox;

const int TEXT_PAGE_N_LINES_MAX=64;

/**
 * This class provides a window in which text can be displayed on screen
 */
class TextPage : public MirrorContainer {
public:
	TextPage(DrawText *drawText, float centerX, float upperY, float width, float height);
	virtual ~TextPage();

	virtual void unlinkAllChildObjects();

	virtual void clearText();
	virtual void addTextAbove(const char *displayStr, const char* replyStr=NULL); // add text at top
	virtual void addTextAboveW(const wchar_t *displayStr, const char* replyStr=NULL); // add text at top
	virtual void addText(const char *displayStr, const char* replyStr=NULL); // add text at bottom
	virtual void addTextW(const wchar_t *displayStr, const char* replyStr=NULL); // add text at bottom
	virtual const char* isButton(float f);
	virtual void setUp();
	virtual int visibleLines();
	virtual float calculateStep();
	virtual void linkSelf(MirrorBase *parentPtr);


private:
	DrawText *drawText;
	Text2DBox *text_boxes[TEXT_PAGE_N_LINES_MAX]; // These are the lines we can show on screen.
	//Text2DBox *textInputBox; // TODO: move this out of the TextPage class.
	int nVisibleTextLines; // How many lines that we have on screen.
	int textLine; // This is the line where we add text, the bottom line.

public:

	/**
	 *  xPos/yPos this is the screen position of upper left corner of the page, given in open GL coordinates.
	 *  No xPos is the center of the window while yPos is the upper edge. A little code cleanup needed...
	 *  TODO Make yPos be the center of the text page same as for yPos.
	 */
	const float xPos;
	const float yPos;

	/**
	 *  height/width is how big the text window is on screen, in open GL style (that is 2.0 would take up the entire screen).
	 */
	const float height;
	const float width;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	TextPage(const TextPage&);
	TextPage& operator=(const TextPage&);


};


//extern TextPage *textPage;
//extern TextPage *tabPage;

#endif /* TEXTPAGE_H_ */
