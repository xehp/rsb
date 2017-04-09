/*
RsbString.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include <stdlib.h>
#include <string.h>

#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif

#include "RsbString.h"

// This allocates memory and copies a string to it.
// Remember to do free on the memory when no longer needed.
/*static char* stringCopyFactory(const char *str)
{
	const int len=strlen(str)+1; // +1 for trailing zero
	char* ptr = (char*)malloc(len);
	memcpy(ptr, str, len);
	return ptr;
}*/



RsbString::RsbString() :
	MirrorContainer(),
	value(NULL)
{

}

RsbString::~RsbString()
{
	stringCopyFreeW(value);
}


void RsbString::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	const wchar_t* ptr = wr->readTokenW();

	setValueW(ptr);

}

#ifndef __EMSCRIPTEN__
void RsbString::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);
	ww->writeStringW(value);
}
#endif


void RsbString::setValueW(const wchar_t* v)
{
	if (v==NULL)
	{
		stringCopyFreeW(value);
		value=NULL;
	}
	else if (value==NULL)
	{
		value = stringCopyFactoryW(v); // stringCopyFree is done in destructor
	}
	else if (wcscmp(v, value)!=0)
	{
		stringCopyFreeW(value);
		value = stringCopyFactoryW(v); // stringCopyFree is done in destructor
		// If this was server side it would be important to update changed counter.
	}
	else
	{
		// do nothing, value is not changed
	}
}


