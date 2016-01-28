/*
 * InputBuffer.cpp
 *
 *  Created on: Apr 30, 2015
 *      Author: henrik
 *
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 */

#include "includeGlEtc.h"
#include "HybridLib.h"
#include "Text2DBox.h"
#include "TextPageScroll.h"
#include "InputBuffer.h"


InputBuffer::InputBuffer()
:	MirrorContainer(),
 	textInputBox(NULL)
{
	inputMode=InputModeIdle;
	for(int i=0;i<sizeof(inputBuffer);i++)
	{
		prevInputBuffer[i] = 0;
		inputBuffer[i] = 0;
	}
	inputBufferIndex = 0; // This tells how many character we have in inputBuffer
	inputBufferPrefix=NULL;
}


InputBuffer::~InputBuffer()
{
	printf("~InputBuffer: %s\n", this->getName());

	unlinkAllChildObjects();
	assert(textInputBox==NULL);
}


void InputBuffer::inputEnterStringEntryMode()
{
	inputMode = InputModeEnteringString;
	inputBufferIndex=0;
	inputBuffer[inputBufferIndex] = 0;


	if (textInputBox!=NULL)
	{
		textInputBox->clearText();
		textInputBox->setText("<enter text here>");
	}

	SDL_StartTextInput();
}

void InputBuffer::inputEnterButtonEntryMode()
{
	inputMode = InputModeEnteringButton;
	inputBufferIndex=0;
	inputBuffer[inputBufferIndex] = 0;

	if (textInputBox!=NULL)
	{
		textInputBox->clearText();
	}

	SDL_StartTextInput();
}

void InputBuffer::inputEnterKeyEntryMode()
{
	inputMode = InputModeEnteringKey;
	inputBufferIndex=0;
	inputBuffer[inputBufferIndex] = 0;

	if (textInputBox!=NULL)
	{
		textInputBox->clearText();
	}
}


void InputBuffer::AddChar(int ch)
{
	if (inputBufferIndex<sizeof(inputBuffer)-1) // -1 because don't use very last position in inputBuffer, its needed for zero to terminate string.
	{
		inputBuffer[inputBufferIndex++] = ch;
		inputBuffer[inputBufferIndex] = 0;
	}
	else
	{
		printf("inputBuffer is full");
	}

	if (textInputBox!=NULL)
	{
		textInputBox->setText(inputBuffer);
	}
}

void InputBuffer::inputBufferClear()
{

	clearText();
	SDL_StopTextInput();

}


void InputBuffer::DelChar()
{
	if (inputBufferIndex>0)
	{
		inputBufferIndex--;
		inputBuffer[inputBufferIndex] = 0;
	}

	if (textInputBox!=NULL)
	{
		textInputBox->setText(inputBuffer);
	}
}



void InputBuffer::DoneNowSend()
{
	if (inputBufferIndex>0)
	{
		if (inputMode!=InputModeIdle)
		{
			char *tmp = (char *)malloc(sizeof(inputBuffer)+10); // free is done just some lines down.
			assert(tmp!=NULL);
			if (inputBufferPrefix==NULL)
			{
				sprintf(tmp, "\"%s\"", inputBuffer);
				textMessagesPage->clearText();
			}
			else
			{
				sprintf(tmp, "%s \"%s\"", inputBufferPrefix, inputBuffer);
				inputBufferPrefix=NULL;
			}
			mainSend(tmp);
			free(tmp); // don't forget this
		}
		else
		{
			//mainSend(inputBuffer);
		}

		inputBufferClear();
	}
}


InputMode InputBuffer::GetMode()
{
	return inputMode;
}

void InputBuffer::SetPrefix(const char* str)
{
	inputBufferPrefix = str;
}





void InputBuffer::unlinkAllChildObjects()
{
	MirrorContainer::unlinkAllChildObjects();

	// textInputBox is just a pointer to a child objects. All child objects should be deleted by MirrorContainer::unlinkAllChildObjects() so it can be set to null now.
	textInputBox=NULL;
}



void InputBuffer::clearText()
{
	strncpy(prevInputBuffer, inputBuffer, sizeof(prevInputBuffer));

	for(int i=0;(i<sizeof(inputBuffer)) && (inputBuffer[i]!=0);i++)
	{
		inputBuffer[i] = 0;
	}

	inputBufferIndex = 0; // This tells how many character we have in inputBuffer
	inputBufferPrefix=NULL;
	inputMode = InputModeIdle;

	if (textInputBox!=NULL)
	{
		textInputBox->clearText();
	}
}



void InputBuffer::setUp()
{
	float step=TextPage::calculateStep();
	textInputBox = new Text2DBox(0.0f, -0.95-(step*0.5f) + step, 1.9f, step);
	assert(textInputBox!=NULL);
	textInputBox->parentId=this->id;
	textInputBox->linkSelfIntoDb(this->mirrorDb);
}

void InputBuffer::getPrev()
{
	inputBufferIndex=0;
	while((prevInputBuffer[inputBufferIndex]!=0) && (inputBufferIndex<sizeof(inputBuffer)))
	{
		inputBuffer[inputBufferIndex]=prevInputBuffer[inputBufferIndex];
		++inputBufferIndex;
	}

	if (inputBufferIndex<sizeof(inputBuffer))
	{
		inputBuffer[inputBufferIndex]=0;
	}

	if (textInputBox!=NULL)
	{
		textInputBox->setText(inputBuffer);
	}

}




