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
#include "Text2DBox.h"
#include "TextPage.h"



TextPage::TextPage() :
MirrorContainer(),
//textInputBox(NULL),
textLine(0),
xPos(0.95f),
yPos(0.99f),
height(1.9f),
width(1.9f)

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

	// text_boxes are child objects and deleted during unlinkSelf
	for (unsigned int i=0;i<SIZEOF_ARRAY(text_boxes);++i)
	{
		text_boxes[i]=NULL;
	}
	//textInputBox=NULL;
}



void TextPage::clearText()
{
	textLine=0;

	for (unsigned int i=0;i<SIZEOF_ARRAY(text_boxes);++i)
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
void TextPage::addText(const char *displayStr, const char* replyStr)
{

	if (textLine >= SIZEOF_ARRAY(text_boxes))
	{
		// Move existing text up (scroll)
		text_boxes[0]->clearText();

		for(int i=0; i<SIZEOF_ARRAY(text_boxes)-1;++i)
		{
			text_boxes[i]->texture_id = text_boxes[i+1]->texture_id;
			text_boxes[i]->buttonCommand = text_boxes[i+1]->buttonCommand;
		}
		--textLine;
		text_boxes[textLine]->texture_id = ZERO_GL;
		text_boxes[textLine]->buttonCommand = NULL;
	}

	assert(textLine<SIZEOF_ARRAY(text_boxes));

	text_boxes[textLine]->setText(displayStr);
	text_boxes[textLine]->setButtonCommand(replyStr);
	++textLine;
}

// Add text at the top
// displayStr : The text to be shown
// replyStr : The command string to be sent if there is a mouse click on the text
void TextPage::addTextAbove(const char *displayStr, const char* replyStr)
{
	assert(textLine>=0);
	assert(textLine<=SIZEOF_ARRAY(text_boxes));

	if (textLine>=SIZEOF_ARRAY(text_boxes))
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
		text_boxes[i]->texture_id = text_boxes[i-1]->texture_id;
		text_boxes[i]->buttonCommand = text_boxes[i-1]->buttonCommand;
	}
	text_boxes[0]->texture_id = ZERO_GL;
	text_boxes[0]->buttonCommand = NULL;


	text_boxes[0]->setText(displayStr);
	text_boxes[0]->setButtonCommand(replyStr);

}


// Returns a string with the command to do if there was a button on the line given by f (f is y coordinates between -1 and 1).
// Returns NULL if there was no button on that line.
const char* TextPage::isButton(float f)
{
	for(unsigned int i=0; i<SIZEOF_ARRAY(text_boxes);++i)
	{
		Text2DBox *t=text_boxes[i];
		float u = t->y + (t->height * 0.5f);
		float l = t->y - (t->height * 0.5f);

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
	float step = calculateStep();
	for(unsigned int i=0;i<SIZEOF_ARRAY(text_boxes);++i)
	{
		float y = (yPos -(step*0.5f)) - (i * step);
		Text2DBox *t = new Text2DBox(0.0f, y, width, step);
		//t->setName("text2DBox");
		t->parentId=this->id;
		t->linkSelfIntoDb(this->mirrorDb);
		text_boxes[i] = t;
	}

}


int TextPage::visibleLines()
{
	return SIZEOF_ARRAY(text_boxes);
}


float TextPage::calculateStep()
{
	float step = 2.0f / (TEXT_PAGE_N_LINES+2);
	return step;
}
