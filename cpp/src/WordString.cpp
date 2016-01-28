/*
WordString.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "WordString.h"


// This is similar to standard str.split(' ') but it will keep strings together.
//
// Our strings contain sub strings such as this line:
// query login_or_reg "Welcome to RSB" buttonPrompt 4 "Login" "Reg new account" "Recover pw" "Cancel"
// It shall be split up like this:
// query
// login_or_reg
// Welcome to RSB
// buttonPrompt
// 4
// Login
// Reg new account
// Recover pw
// Cancel
//

static bool hlibIsAsciiGraph(int ch)
{
  return ((ch>' ') && (ch<='~'));
}

void WordString::addChar(int ch)
{
	assert(nBytes<bufSize);
	bufPtr[nBytes++] = ch;
}



WordString::WordString(const char *str):
  bufPtr(NULL),
  bufSize(0),
  nBytes(0),
  nWords(0),
  readPos(0)
{
	const unsigned int strLen=strlen(str);
	bufSize = strLen +4; // Make room for last trailing zero

	bufPtr=(char*)malloc(bufSize); // free is done in destructor
	assert(bufPtr!=NULL);


	// Copy the string, replace spaces with zero.
	for (unsigned int i=0;i<strLen;++i)
	{
		// skip space etc, find beginning
		while((i<strLen) && (!hlibIsAsciiGraph(str[i])))
		{
			i++;
		}

		// find the end of word/string

		// Is is a quoted string?
		if (str[i]=='"')
		{
			i++;

			// This is a quoted string, find the end quote.
			while((i<strLen) && (str[i]!='"'))
			{
				// Is it a escape char sequence?
				if(str[i]=='\\')
				{
					// It is.

					printf("escape char sequence is not implemented yet");
					// not implemented yet
					addChar(str[i]);
				}
				else
				{
					addChar(str[i]);
				}

				i++;
			}

			if (str[i]=='"')
			{
				// End quote found.
				i++;
			}

		}
		else
		{
			// Not quoted, just copy until it ends with a non graph char.
			while((i<strLen) && (hlibIsAsciiGraph(str[i])))
			{
				addChar(str[i]);
				++i;
			}
		}

		addChar(0);

	}
}

WordString::~WordString()
{
		free(bufPtr);
}


const char* WordString::lookAtString()
{
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return NULL;
	}

	const char *ptr = bufPtr+readPos;

	return ptr;
}

const char* WordString::getNextString()
{
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return NULL;
	}

	const char *ptr = bufPtr+readPos;

	// Move readPos to next sub string.
	while(bufPtr[readPos])
	{
		++readPos;
	}

	++readPos;

	return ptr;
}

bool WordString::equalsIgnoreCase(const char *str)
{
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return str==NULL;
	}

	const char *ptr = bufPtr+readPos;


	if (strcmp(ptr, str)==0)
	{
		return true;
	}

	return false;
}


int WordString::readInt()
{
	const char *s=getNextString();
	// TODO: check that it really is an integer.
	return atoi(s);
}

long long WordString::readLong()
{
	const char *s=getNextString();
	// TODO: check that it really is an integer.
	return strtoll(s, NULL, 0); // Warning: a string like 010 will be parsed as octal so it gives 8 not 10.
}


float WordString::readFloat()
{
	const char *s=getNextString();
	return atof(s);
}

// http://stackoverflow.com/questions/1012571/stdstring-to-float-or-double
/*
double WordString::readDouble()
{
	const char *s=getNextString();
	return std::stod(s);
}
*/




