/*
 * TextPageScroll.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *  Created on: Jul 20, 2015
 *      Author: drekkar
 */


#include <stdlib.h>
#include <assert.h>
#include "includeGlEtc.h"
#include "MirrorBase.h"
#include "Color2DBox.h"
#include "Color2DFrame.h"
#include "TextPageScroll.h"

TextPageScroll *textMessagesPage=NULL; // This is the regular text page, menu options, messages etc are shown here.
TextPageScroll *textStatisticsPage=NULL; // This is the alternative text page, it is shown when user hold down the tab key.
TextPageScroll *textInventoryPage=NULL; // This is the alternative text page, it is shown when user hold down the tab key.

TextPageScroll *textFocusPage=NULL; // Tells which of the above text pages that is in focus (shall receive input events such as scrolling).

TextRoundBuffer::TextRoundBuffer() :
		head(0),
		tail(0),
		size(0)
{
	assert(SIZEOF_ARRAY(textNames)==SIZEOF_ARRAY(textString));

	for (unsigned int i=0;i<SIZEOF_ARRAY(textNames);++i)
	{
		textNames[i]=NULL;
		textString[i]=NULL;
	}

};


TextRoundBuffer::~TextRoundBuffer()
{
	for (unsigned int i=0;i<SIZEOF_ARRAY(textNames);++i)
	{
		free(textNames[i]);
		free(textString[i]);
	}
}


int TextRoundBuffer::incAndWrap(int i)
{
	if (i==(SIZEOF_ARRAY(textNames)-1))
	{
		return 0;
	}
	return i+1;
}


void TextRoundBuffer::addText(const char *displayStr, const char* replyStr)
{
	if (size==TEXT_BUFFER_SIZE)
	{
		// Buffer is full, remove oldest lines
		free(textString[head]);
		free(textNames[head]);
		textString[head]=NULL;
		head=incAndWrap(head);
	}
	else
	{
		size++;
	}

	assert(textString[tail]==NULL);
	textString[tail]=stringCopyFactory(displayStr);
	textNames[tail]=stringCopyFactory(replyStr);

	tail=incAndWrap(tail);

	if (tail==head)
	{
		// tail have caught up with head, skip the one at head
	}
};


// the text buffer alias display string
// index=0 gives text at head.
const char* TextRoundBuffer::getTextString(unsigned int index)
{
	index+=head;
	index%=TEXT_BUFFER_SIZE;
	return textString[index];
}


// alias reply string
const char* TextRoundBuffer::getTextName(unsigned int index)
{
	index+=head;
	index%=TEXT_BUFFER_SIZE;
	return textNames[index];
}


void TextRoundBuffer::clearText()
{

	for (unsigned int i=0;i<SIZEOF_ARRAY(textNames);++i)
	{
		free(textNames[i]);
		textNames[i]=NULL;
	}

	for (unsigned int i=0;i<SIZEOF_ARRAY(textString);++i)
	{
		free(textString[i]);
		textString[i]=NULL;
	}

	head=0;
	tail=0;
	size=0;

}

TextPageScroll::TextPageScroll() :
	TextPage(),
	scrollPos(0),
	color2DBox(NULL),
	color2DBox2(NULL),
	textRoundBuffer()
{

}


TextPageScroll::~TextPageScroll() {
	clearText();
	if (color2DBox!=NULL)
	{
		delete(color2DBox);
	}
	if (color2DBox2!=NULL)
	{
		delete(color2DBox2);
	}
}


void TextPageScroll::clearText()
{
	TextPage::clearText();

	scrollPos=0;

	textRoundBuffer.clearText();

	setScrollBar();
}


// Add text at the bottom
// displayStr : The text to be shown
// replyStr : The command string to be sent if there is a mouse click on the text
void TextPageScroll::addText(const char *displayStr, const char* replyStr)
{
	bool f=(textRoundBuffer.getSize()==TEXT_BUFFER_SIZE);

	textRoundBuffer.addText(displayStr, replyStr);

	const int v=visibleLines();


	if (!f)
	{
		// The buffer was not full (yet)
		if (textRoundBuffer.getSize()<=v)
		{
			// There are not more lines than there is room for on screen, no scrolling needed.
			TextPage::addText(displayStr, replyStr);
		}
		else if (scrollPos+v+1 == textRoundBuffer.getSize())
		{
			// If scrolled down at bottom, then scroll so we stay at bottom
			TextPage::addText(displayStr, replyStr);
			scrollPos++;
		}
	}
	else
	{
		// The buffer was full
		if (scrollPos+v == textRoundBuffer.getSize())
		{
			// If scrolled down at bottom, then scroll so we stay at bottom
			TextPage::addText(displayStr, replyStr);
		}
		else if (scrollPos==0)
		{
			// scrolled at at top, need to scroll out top line since its no longer in the round buffer
			int p=v-1;
			TextPage::addText(textRoundBuffer.getTextString(p), textRoundBuffer.getTextName(p));
		}
		else if (scrollPos>0)
		{
			// not on the bottom line, but the line we scrolled to is now one step up, adjust scroll pos.
			scrollPos--;
		}
	}

	setScrollBar();

}

// The scroll bar shows how much we scrolled and how much of the total text buffer that is shown.
void TextPageScroll::setScrollBar()
{
	if ((color2DBox!=NULL) && (color2DBox2!=NULL))
	{
		const int v=visibleLines();
		const int s=textRoundBuffer.getSize();

		if (s<=v)
		{
			const float q2=0; //((float)scrollLines)/v;
			color2DBox2->changeSize(0.95f, yPos-0.5f*height*q2, 0.025f, height*q2);

			const float q=0; //((float)scrollLines)/v;
			color2DBox->changeSize(0.95f, yPos-0.5f*height*q, 0.025f, height*q);
		}
		else
		{
			const float q2=1;
			color2DBox2->changeSize(0.95f, yPos-0.5f*height*q2, 0.025f, height*q2);

			const float lenQ=((float)v)/((float)s);
			const float posQ=((float)scrollPos+(v*0.5f))/((float)s);
			color2DBox->changeSize(0.95f, yPos-height*posQ, 0.025f, height*lenQ);
		}
	}
}


// The text (on screen) is moved down, analogous to the view window being moved up.
void TextPageScroll::scrollUp()
{
	if (scrollPos>0)
	{
		--scrollPos;
		const int p=scrollPos;
		TextPage::addTextAbove(textRoundBuffer.getTextString(p), textRoundBuffer.getTextName(p));
	}
	else
	{
		printf("already at top\n");
	}

	setScrollBar();
}


// Same as scrollUp but the other way.
void TextPageScroll::scrollDown()
{
	const int v=visibleLines();
	if ((scrollPos+v)<(textRoundBuffer.getSize()))
	{
		int p=scrollPos+v;
		TextPage::addText(textRoundBuffer.getTextString(p), textRoundBuffer.getTextName(p));
		++scrollPos;
	}
	else
	{
		printf("already at bottom\n");
	}

	setScrollBar();
}


// Set up text areas and scroll bar
void TextPageScroll::setUp()
{
	TextPage::setUp();

	createScrollBar();
}

// Scroll bar consist of two boxes
void TextPageScroll::createScrollBar()
{
	if (color2DBox==NULL)
	{
		//float step = calculateStep();
		color2DBox = new Color2DBox(0.95f, 0.0, 0.025f, 0.0f);
		color2DBox->parentId=this->id;
		color2DBox->linkSelfIntoDb(this->mirrorDb);
	}

	if (color2DBox2==NULL)
	{
		color2DBox2 = new Color2DFrame(0.95f, 0.0, 0.025f, 0.0f);
		color2DBox2->parentId=this->id;
		color2DBox2->linkSelfIntoDb(this->mirrorDb);
	}
}


void TextPageScroll::unlinkAllChildObjects()
{
	clearText();

	TextPage::unlinkAllChildObjects();

	color2DBox=NULL;
	color2DBox2=NULL;

}
