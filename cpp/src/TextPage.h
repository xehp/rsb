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

const int TEXT_PAGE_N_LINES=21;


class TextPage : public MirrorContainer {
public:
	TextPage();
	virtual ~TextPage();

	virtual void unlinkAllChildObjects();

	virtual void clearText();
	virtual void addTextAbove(const char *displayStr, const char* replyStr=NULL); // add text at top
	virtual void addText(const char *displayStr, const char* replyStr=NULL); // add text at bottom
	virtual const char* isButton(float f);
	virtual void setUp();
	virtual int visibleLines();
	static float calculateStep();


	Text2DBox *text_boxes[TEXT_PAGE_N_LINES]; // These are the lines we can show on screen.
	//Text2DBox *textInputBox; // TODO: move this out of the TextPage class.
	int textLine; // This is the line where we add text, the bottom line.

	const float xPos;
	const float yPos;
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
