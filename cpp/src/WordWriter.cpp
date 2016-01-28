/*
WordWriter.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include <cctype> // http://www.cplusplus.com/reference/cctype/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "WordWriter.h"




WordWriter::WordWriter(size_t bufSize):
  bufPtr(NULL),
  bufSize(bufSize),
  nBytes(0),
  compleated(false)
{
	bufPtr=(char*)malloc(bufSize+2); // free is to be done in destructor "free(bufPtr);"
	assert(bufPtr!=NULL);
	bufPtr[nBytes]=0;
}


WordWriter::~WordWriter()
{
	free(bufPtr);
}


void WordWriter::addChar(int ch)
{
	if (nBytes>=bufSize)
	{
		// buffer is full, make a larger buffer
		char *tmpPtr=(char*)malloc((bufSize*2)+2); // free is done some lines down in this method
		assert(tmpPtr!=NULL);
		memcpy(tmpPtr, bufPtr, bufSize);
		free(bufPtr);
		bufPtr = tmpPtr;
		bufSize *= 2;
	}

	bufPtr[nBytes]=ch;
	++nBytes;
}


const char *WordWriter::getString()
{
	compleated=true;
	return bufPtr;
}


void WordWriter::writeWord(const char* str)
{
	assert(compleated==false);

	if (nBytes)
	{
		addChar(' ');
	}

	while(*str)
	{
		addChar(*str);
		++str;
	}

	assert(nBytes<bufSize);
	bufPtr[nBytes]=0;
}

void WordWriter::writeString(const char* str)
{
	assert(compleated==false);

	if (nBytes)
	{
		addChar(' ');
	}

	addChar('"');

	for(;;)
	{
		const int ch = *str;
		if ((ch == '"') || (ch == '\\'))
		{
			addChar('\\');
		}
		addChar(ch);
		++str;
	}

	addChar('"');

	assert(nBytes<bufSize);
	bufPtr[nBytes]=0;
}


void WordWriter::writeInt(int i)
{
	char str[40];
	sprintf(str, "%d",i);
	writeWord(str);
}

void WordWriter::writeLong(long long i)
{
	char str[40];
	sprintf(str, "%lld",i); // TODO optimize code, http://stackoverflow.com/questions/3958449/unsigned-long-long-to-string-convert
	writeWord(str);
}

void WordWriter::writeFloat(float f)
{
	char str[64];
	sprintf(str, "%f",f);
	writeWord(str);
}




