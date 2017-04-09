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
#include "WordWriter.h"
#include "HybridLib.h"
#include "Text2DBox.h"
#include "TextPageScroll.h"
#include "DrawText.h"
#include "InputBuffer.h"


InputBuffer::InputBuffer(DrawText *drawText, float centerX, float centerY, float width, float height)
:	MirrorContainer(),
	drawText(drawText),
	textInputBox(NULL),
	xPos(centerX),
	yPos(centerY),
	width(width),
	height(height)
{
	printf("InputBuffer\n");
	inputMode=InputModeFirstPerson;
	for(int i=0;i<SIZEOF_ARRAY(inputBuffer);i++)
	{
		inputBuffer[i] = 0;
	}
	for(int i=0;i<SIZEOF_ARRAY(prevInputBuffer);i++)
	{
		prevInputBuffer[i] = 0;
	}
	inputBufferLen = 0; // This tells how many character we have in inputBuffer
	inputBufferPos = 0; // This is where next character is to be written (curser position if we had one)
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
	printf("InputBuffer::inputEnterStringEntryMode\n");
	inputMode = InputModeEnteringTextString;
	inputBufferLen=0;
	inputBufferPos=0;
	inputBuffer[inputBufferLen] = 0;


	if (textInputBox!=NULL)
	{
		textInputBox->clearText();
		textInputBox->setText("<enter text here>");
	}

	SDL_StartTextInput();
}

void InputBuffer::inputEnterButtonEntryMode()
{
	printf("InputBuffer::inputEnterButtonEntryMode\n");
	inputMode = InputModeEnteringButtonOrMenu;
	inputBufferLen=0;
	inputBufferPos=0;
	inputBuffer[inputBufferLen] = 0;

	if (textInputBox!=NULL)
	{
		textInputBox->clearText();
	}

	SDL_StartTextInput();
}

void InputBuffer::inputEnterKeyEntryMode()
{
	printf("InputBuffer::inputEnterKeyEntryMode\n");
	inputMode = InputModeEnteringPhysicalKey;
	inputBufferLen=0;
	inputBufferPos=0;
	inputBuffer[inputBufferLen] = 0;

	if (textInputBox!=NULL)
	{
		textInputBox->clearText();
	}
}


void InputBuffer::AddChar(int ch)
{
	printf("InputBuffer::AddChar %d\n",ch);
	if (inputBufferLen<sizeof(inputBuffer)-1) // -1 because don't use very last position in inputBuffer, its needed for zero to terminate string.
	{
		if (inputBufferPos!=inputBufferLen)
		{
			assert(inputBufferPos>=0);
			assert(inputBufferLen>=0);
			assert(inputBufferPos<=inputBufferLen);
			// Move all characters after inputBufferPos one step to right
			for(int i=inputBufferLen;i>inputBufferPos;--i)
			//for(int i=inputBufferPos;i<inputBufferLen;++i)
			{
				assert(i<SIZEOF_ARRAY(inputBuffer));
				assert((i-1)>=0);
				inputBuffer[i] = inputBuffer[i-1];
			}
		}
		inputBuffer[inputBufferPos] = ch;
		inputBufferPos++;
		inputBufferLen++;
		inputBuffer[inputBufferLen] = 0;
	}
	else
	{
		printf("inputBuffer is full");
	}

	if (textInputBox!=NULL)
	{
		textInputBox->setTextW(inputBuffer);
	}
}

void InputBuffer::inputBufferClear()
{
	printf("InputBuffer::inputBufferClear\n");
	clearText();
	SDL_StopTextInput();
}


void InputBuffer::DelChar()
{
	printf("InputBuffer::DelChar\n");
	if (inputBufferLen>0)
	{
		/*if (inputBufferPos<inputBufferLen)
		{
			inputBufferPos++;
		}*/
		if (inputBufferPos>0)
		{
			inputBufferPos--;
		}

		assert(inputBufferLen>=0);
		assert(inputBufferPos<=inputBufferLen);

		// Move all characters after inputBufferPos one step to left
		for(int i=inputBufferPos;i<inputBufferLen;++i)
		//for(int i=inputBufferLen;i<inputBufferPos;--i)
		{
			assert((i+1)<SIZEOF_ARRAY(inputBuffer));
			inputBuffer[i] = inputBuffer[i+1];
		}
		inputBufferLen--;
		inputBuffer[inputBufferLen] = 0;
	}

	if (textInputBox!=NULL)
	{
		// Update the text visible to user
		textInputBox->setTextW(inputBuffer);
	}
}

// This is called when return/enter has been pressed, the string is sent to server.
// TODO sending to server should not be part of InputBuffer. The collected string should be just retrieved by the using code and sent from there. This so the input buffer can be used for other things than sending a reply to server.
// Deprecated, get the text with getText(), encode it and then send it by calling mainSend
void InputBuffer::DoneNowSend()
{
	if (inputBufferLen>0)
	{
		if (inputMode!=InputModeFirstPerson)
		{
			if (inputBufferPrefix==NULL)
			{
				const int s=64+SIZEOF_ARRAY(inputBuffer)*4+8;
				char *tmp = (char *)malloc(s+4); // free is done just some lines down.
				assert(tmp!=NULL);
				tmp[0]='\"';
				const int n=WordWriter::encodeStringW(tmp+1, inputBuffer, s);
				assert(n+2<s+8);
				tmp[n+1]='\"';
				tmp[n+2]=0;
				mainSend(tmp);
				free(tmp); // don't forget this
			}
			else
			{
				const int s=strlen(inputBufferPrefix)+SIZEOF_ARRAY(inputBuffer)*4+8;
				char *tmp = (char *)malloc(s+4); // free is done just some lines down.
				assert(tmp!=NULL);
				//sprintf(tmp, "%s \"%s\"", inputBufferPrefix, inputBuffer);
				int n=WordWriter::encodeString(tmp, inputBufferPrefix, s);
				tmp[n]=' ';
				++n;
				tmp[n]='\"';
				++n;
				n+=WordWriter::encodeStringW(tmp+n, inputBuffer, s-n);
				assert(n+2<SIZEOF_ARRAY(inputBuffer)*4+8);
				tmp[n+1]='\"';
				tmp[n+2]=0;
				inputBufferPrefix=NULL;
				mainSend(tmp);
				free(tmp); // don't forget this
			}
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
	printf("InputBuffer::SetPrefix '%s'\n",str);
	inputBufferPrefix = str;
}


void InputBuffer::unlinkAllChildObjects()
{
	printf("InputBuffer::unlinkAllChildObjects\n");

	MirrorContainer::unlinkAllChildObjects();

	// textInputBox is just a pointer to a child objects. All child objects should be deleted by MirrorContainer::unlinkAllChildObjects() so it can be set to null now.
	textInputBox=NULL;
}


void InputBuffer::clearText()
{
	printf("InputBuffer::clearText\n");

	wcsncpy(prevInputBuffer, inputBuffer, SIZEOF_ARRAY(prevInputBuffer));

	for(int i=0;(i<SIZEOF_ARRAY(inputBuffer)) && (inputBuffer[i]!=0);i++)
	{
		inputBuffer[i] = 0;
	}

	inputBufferLen = 0; // This tells how many character we have in inputBuffer
	inputBufferPos = 0;
	inputBufferPrefix=NULL;
	inputMode = InputModeFirstPerson;

	if (textInputBox!=NULL)
	{
		textInputBox->clearText();
	}
}


void InputBuffer::setUp()
{
	printf("InputBuffer::setUp\n");

	//float step=TextPage::calculateStep();
	//float step=calculateStep();
	//textInputBox = new Text2DBox(xPos, yPos-(step*0.5f) + step, width, step);
	textInputBox = new Text2DBox(drawText, xPos, yPos, width, height);
	assert(textInputBox!=NULL);
	textInputBox->linkSelf(this);
}


void InputBuffer::getPrev()
{
	printf("InputBuffer::getPrev\n");

	inputBufferLen=0;
	while((prevInputBuffer[inputBufferLen]!=0) && (inputBufferLen<SIZEOF_ARRAY(inputBuffer)))
	{
		inputBuffer[inputBufferLen]=prevInputBuffer[inputBufferLen];
		++inputBufferLen;
	}

	if (inputBufferLen<SIZEOF_ARRAY(inputBuffer))
	{
		inputBuffer[inputBufferLen]=0;
	}

	inputBufferPos=inputBufferLen;

	if (textInputBox!=NULL)
	{
		textInputBox->setTextW(inputBuffer);
	}

}

void InputBuffer::MoveCursorToEnd()
{
	printf("InputBuffer::MoveCursorToEnd\n");
	inputBufferPos=inputBufferLen;
}

void InputBuffer::MoveCursorBack()
{
	printf("InputBuffer::MoveCursorBack\n");
	if (inputBufferPos>0)
	{
		inputBufferPos--;
	}
	printf("MoveCursorBack %d %d\n",inputBufferPos,inputBufferLen);
}

void InputBuffer::MoveCursorForward()
{
	printf("InputBuffer::MoveCursorForward\n");
	if (inputBufferPos<inputBufferLen)
	{
		inputBufferPos++;
	}
	printf("MoveCursorForward %d %d\n",inputBufferPos,inputBufferLen);
}


float InputBuffer::calculateStep()
{
	float step = height;
	return step;
}





