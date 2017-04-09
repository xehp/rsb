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


// See also decodeHex in WordReader
static int encodeHex(unsigned int c)
{
	if (c<10)
	{
		return '0'+c;
	}
	else if (c<=15)
	{
		return 'A'-10+c;
	}
	else
	{
		return '?';
	}
}



WordWriter::WordWriter(size_t bufSize):
  bufPtr(NULL),
  bufSize(bufSize),
  nBytes(0),
  compleated(false),
  needSeparator(false),
  separatorChar(' '),
  indentationLevel(0)
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
	if (nBytes==bufSize)
	{
		// buffer is full, make a larger buffer
		assert(nBytes<=bufSize);
		char *tmpPtr=(char*)malloc((bufSize*2)+2); // free of the old buffer is done some lines down in this method, free on the new buffer is done in destructor.
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
	compleated=true; // Once the string has been fetched no more writing is allowed.
	addChar(0); // Terminate the string.
	assert(nBytes<=bufSize);
	return bufPtr;
}


void WordWriter::writeWord(const char* str)
{
	assert(compleated==false);

	addSeparatorIfNeeded();

	while(*str)
	{
		addChar(*str);
		++str;
	}

	needSeparator = true;
}

void WordWriter::writeString(const char* str)
{
	assert(compleated==false);

	addSeparatorIfNeeded();

	addChar('"');

	while(*str)
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

	needSeparator = true;
}

void WordWriter::writeStringW(const wchar_t* str)
{
	assert(compleated==false);

	addSeparatorIfNeeded();

	addChar('"');

	while(*str)
	{
		const int ch = *str;

		if ((ch == '"') || (ch == '\\'))
		{
			addChar('\\');
		}

		if (ch>=256)
		{
			addChar('\\');
			addChar('u');
			addChar(encodeHex((ch>>16)&0xf));
			addChar(encodeHex((ch>>8)&0xf));
			addChar(encodeHex((ch>>4)&0xf));
			addChar(encodeHex(ch&0xf));

		}
		else if (ch>=128)
		{
			addChar('\\');
			addChar('x');
			addChar(encodeHex((ch>>4)&0xf));
			addChar(encodeHex(ch&0xf));
		}
		else
		{
			addChar(ch);
		}


		++str;
	}

	addChar('"');

	needSeparator = true;
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

void WordWriter::addSeparatorIfNeeded()
{
	if (needSeparator)
	{
		addChar(separatorChar);
		needSeparator=false;
	}
}

void WordWriter::addNewLine()
{
	if (indentationLevel>0)
	{
		addChar('\n');
		for(int i=0;i<indentationLevel;++i)
		{
			addChar(' ');
		}
	}
}



// Returns number of characters written to dst, not including terminating zero.
// See also decodeString in WordReader
// NOTE the string will not always be null terminated
int WordWriter::encodeString(char *dst, const char *src, int n)
{
	char *orgDst=dst;

	while((*src) && (n>0))
	{
		unsigned int ch=*src;
		//if (ch>=256) ... else
		if (ch>=128)
		{
			assert(n>=4);
			n-=4;
			*dst++='\\';
			*dst++='x';
			*dst++=encodeHex((ch>>4)&0xf);
			*dst++=encodeHex(ch&0xf);
		}
		else
		{
			assert(n>0);
			*dst++=ch;
			--n;
		}
		++src;
	}

	if(n>0)
	{
		*dst=0;
	}
	return dst-orgDst;
}

// Returns number of characters written to dst, not including terminating zero.
// dstSize shall be the size of dst buffer, not number of characters to encode.
// See also decodeString in WordReader
// NOTE the string will not always be null terminated
int WordWriter::encodeStringW(char *dst, const wchar_t *src, int dstSize)
{
	char *orgDst=dst;

	while((*src) && (dstSize>0))
	{
		unsigned int ch=*src;
		if (ch>=256)
		{
			assert(dstSize>=4);
			dstSize-=4;
			*dst++='\\';
			*dst++='u';
			*dst++=encodeHex((ch>>16)&0xf);
			*dst++=encodeHex((ch>>8)&0xf);
			*dst++=encodeHex((ch>>4)&0xf);
			*dst++=encodeHex(ch&0xf);

		}
		else if (ch>=128)
		{
			assert(dstSize>=4);
			dstSize-=4;
			*dst++='\\';
			*dst++='x';
			*dst++=encodeHex((ch>>4)&0xf);
			*dst++=encodeHex(ch&0xf);
		}
		else
		{
			assert(dstSize>0);
			*dst++=ch;
			--dstSize;
		}
		++src;
	}

	if(dstSize>0)
	{
		*dst=0;
	}
	return dst-orgDst;
}


void WordWriter::writeString(const std::string& str)
{
	writeString(str.c_str());
}

void WordWriter::writeWord(const std::string& str)
{
	writeWord(str.c_str());
}

