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
	free(value);
}


void RsbString::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);

	free(value);
	value = NULL;

	const char* ptr = wr->readString();

	if (ptr!=NULL)
	{
		value=stringCopyFactory(ptr);
	}

}

void RsbString::setValue(const char* v)
{
	free(value);
	value = stringCopyFactory(v);
}


