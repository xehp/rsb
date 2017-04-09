/*
 * TextPage.cpp
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 */

#include "includeGlEtc.h"
#include "WindowObject.h"
#include "DrawText.h"
#include "Text2DBox.h"
#include "TextPage.h"

/**
 * This class keeps a number of images with text.
 * Only as many lines as visible are kept here.
 * It is used by the TextPageScroll which provides a bigger page then the number of lines on screen.
 */

TextPage::TextPage(DrawText *drawText, float centerX, float upperY, float width, float height) :
MirrorContainer(),
//textInputBox(NULL),
drawText(drawText),
nVisibleTextLines(0),
textLine(0),
xPos(centerX),
yPos(upperY),
height(height),
width(width)
{
	for (unsigned int i=0;i<SIZEOF_ARRAY(text_boxes);++i)
	{
		text_boxes[i]=NULL;
	}
}


TextPage::~TextPage()
{
	unlinkAllChildObjects();
	assert(text_boxes[0]==NULL);
}

void TextPage::unlinkAllChildObjects()
{
	MirrorContainer::unlinkAllChildObjects();

	// text_boxes are also child objects and deleted by super class (MirrorContainer::unlinkAllChildObjects) so all we need to do here is clear the pointer.
	for (unsigned int i=0;i<nVisibleTextLines;++i)
	{
		text_boxes[i]=NULL;
	}
	//textInputBox=NULL;
}



void TextPage::clearText()
{
	textLine=0;

	for (unsigned int i=0;i<nVisibleTextLines;++i)
	{
		if (text_boxes[i]!=NULL)
		{
			text_boxes[i]->clearText();
		}
	}


}


// Add text at the bottom
// displayStr : The text to be shown
// replyStr : The command string to be sent if there is a mouse click on the text
// Deprecated, use addTextW
void TextPage::addText(const char *displayStr, const char* replyStr)
{
	if (textLine >= nVisibleTextLines)
	{
		// Move existing text up (scroll)
		text_boxes[0]->clearText();

		for(int i=0; i<nVisibleTextLines-1;++i)
		{
			text_boxes[i]->texture_id = text_boxes[i+1]->texture_id;
			text_boxes[i]->buttonCommand = text_boxes[i+1]->buttonCommand;
		}
		--textLine;
		text_boxes[textLine]->texture_id = ZERO_GL;
		text_boxes[textLine]->buttonCommand = NULL;
	}

	assert(textLine<nVisibleTextLines);

	text_boxes[textLine]->setText(displayStr);
	text_boxes[textLine]->setButtonCommand(replyStr);
	++textLine;
}

void TextPage::addTextW(const wchar_t *displayStr, const char* replyStr)
{
	if (textLine >= nVisibleTextLines)
	{
		assert(text_boxes[0]!=NULL);
		assert(nVisibleTextLines<=SIZEOF_ARRAY(text_boxes));

		// Move existing text up (scroll)
		text_boxes[0]->clearText();

		for(int i=0; i<nVisibleTextLines-1;++i)
		{
			assert(text_boxes[i]!=NULL);
			text_boxes[i]->texture_id = text_boxes[i+1]->texture_id;
			text_boxes[i]->buttonCommand = text_boxes[i+1]->buttonCommand;
		}
		--textLine;
		text_boxes[textLine]->texture_id = ZERO_GL;
		text_boxes[textLine]->buttonCommand = NULL;
	}

	assert(textLine<nVisibleTextLines);

	text_boxes[textLine]->setTextW(displayStr);
	text_boxes[textLine]->setButtonCommand(replyStr);
	++textLine;
}

// Add text at the top
// displayStr : The text to be shown
// replyStr : The command string to be sent if there is a mouse click on the text
// Deprecated, use addTextAboveW
void TextPage::addTextAbove(const char *displayStr, const char* replyStr)
{
	assert(textLine>=0);
	assert(textLine<=nVisibleTextLines);

	if (textLine>=nVisibleTextLines)
	{
		// Move existing text down (scroll)
		text_boxes[textLine-1]->clearText();
	}
	else
	{
		++textLine;
	}


	for(int i=textLine-1; i>=1;--i)
	{
		assert(i<SIZEOF_ARRAY(text_boxes));
		assert(text_boxes[i]!=NULL);

		text_boxes[i]->texture_id = text_boxes[i-1]->texture_id;
		text_boxes[i]->buttonCommand = text_boxes[i-1]->buttonCommand;
	}
	text_boxes[0]->texture_id = ZERO_GL;
	text_boxes[0]->buttonCommand = NULL;


	text_boxes[0]->setText(displayStr);
	text_boxes[0]->setButtonCommand(replyStr);

}


// Add text at the top
// displayStr : The text to be shown
// replyStr : The command string to be sent if there is a mouse click on the text
void TextPage::addTextAboveW(const wchar_t *displayStr, const char* replyStr)
{
	assert(textLine>=0);
	assert(textLine<=nVisibleTextLines);

	if (textLine>=nVisibleTextLines)
	{
		// Move existing text down (scroll)
		text_boxes[textLine-1]->clearText();
	}
	else
	{
		++textLine;
	}


	for(int i=textLine-1; i>=1;--i)
	{
		assert(i<SIZEOF_ARRAY(text_boxes));
		assert(text_boxes[i]!=NULL);

		text_boxes[i]->texture_id = text_boxes[i-1]->texture_id;
		text_boxes[i]->buttonCommand = text_boxes[i-1]->buttonCommand;
	}
	text_boxes[0]->texture_id = ZERO_GL;
	text_boxes[0]->buttonCommand = NULL;


	text_boxes[0]->setTextW(displayStr);
	text_boxes[0]->setButtonCommand(replyStr);

}

// Returns a string with the command to do if there was a button on the line given by f (f is y coordinates between -1 and 1).
// Returns NULL if there was no button on that line.
const char* TextPage::isButton(float f)
{
	for(unsigned int i=0; i<nVisibleTextLines;++i)
	{
		Text2DBox *t=text_boxes[i];
		assert(t!=NULL);


		float u = t->yCenterPos + (t->height * 0.5f);
		float l = t->yCenterPos - (t->height * 0.5f);

		printf("isButton %f %d %f %f '%ls' '%s'\n", f, i, u, l, text_boxes[i]->debugTextString , text_boxes[i]->buttonCommand);
		fflush(stdout);

		if ((f<u) && (f>l))
		{
			if (text_boxes[i]->buttonCommand!=NULL)
			{
				return text_boxes[i]->buttonCommand;
			}
		}
	}
	return NULL;
}


void TextPage::setUp()
{
	// Set up text areas
	const float step = calculateStep();
	for(unsigned int i=0;i<nVisibleTextLines;++i)
	{
		float y = (yPos -(step*0.5f)) - (i * step);
		Text2DBox *t = new Text2DBox(drawText, xPos, y, width, step);
		//t->setName("text2DBox");
		t->linkSelf(this);
		text_boxes[i] = t;
	}

}


int TextPage::visibleLines()
{
	return nVisibleTextLines;
}

/**
 * How much distance between each line of text, given in Open GL style that is 2.0 would be the entire screen
 */
float TextPage::calculateStep()
{
	//float step = height / (TEXT_PAGE_N_LINES);

	WindowObject *windowObject = getWindowObject();
	assert(windowObject!=NULL);
	float step = windowObject->scale2dPixelToGlFloatY(drawText->getSizeY());
	return step;
}


void TextPage::linkSelf(MirrorBase *parentPtr)
{
	MirrorContainer::linkSelf(parentPtr);


	WindowObject *windowObject = getWindowObject();
	assert(windowObject!=NULL);

	// Calculate how many pixels the window/page has.
	int ySize=floor(windowObject->scaleGlFloatTo2dPixelY(height));
	int yStep=drawText->getSizeY();  // round(Text2DBox::scaleGlFloatTo2dPixelY(calculateStep()));

	// So how many lines will fit.
	nVisibleTextLines=floor(ySize/yStep);
	if (nVisibleTextLines>SIZEOF_ARRAY(text_boxes))
	{
		nVisibleTextLines=SIZEOF_ARRAY(text_boxes);
	}
	//nVisibleTextLines=SIZEOF_ARRAY(text_boxes);

}


