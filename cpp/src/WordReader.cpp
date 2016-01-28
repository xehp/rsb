/*
WordReader.cpp

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
#include "WordReader.h"


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

/*
void WordReader::addChar(int ch)
{
	assert(nBytes<bufSize);
	bufPtr[nBytes++] = ch;
}
*/


WordReader::WordReader(const char *str):
  bufPtr(NULL),
  bufSize(0),
  nBytes(0),
  nWords(0),
  readPos(0)
{
	const unsigned int strLen=strlen(str);
	bufSize = strLen+1; // Make room for last trailing zero

	bufPtr=(char*)malloc(bufSize); // free is done in destructor: "free(bufPtr)".
	assert(bufPtr!=NULL);
	memcpy(bufPtr, str, bufSize);
}

WordReader::~WordReader()
{
	free(bufPtr);
}


const char* WordReader::lookAtString() const
{
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return NULL;
	}

	const char *ptr = bufPtr+readPos;

	return ptr;
}

// Read a word, a word is a number of characters separated by space (or linefeed etc).
// NOTE! If the string needs to be saved it must be copied. This will return a pointer to an internal memory that will be reused by the time of next call to WordReader.
const char* WordReader::readWord()
{
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return NULL;
	}

	// Skip leading spaces, if any.
	while(isspace(bufPtr[readPos]))
	{
		++readPos;
	}


	const char *ptr = bufPtr+readPos;

	// Move readPos to next sub string.
	while(isgraph(bufPtr[readPos]))
	{
		++readPos;
	}

	bufPtr[readPos]=0;

	++readPos;

	return ptr;
}

// Same as readWord but reads lines
// NOTE! If the string needs to be saved it must be copied. This will return a pointer to an internal memory that will be reused by the time of next call to WordReader.
const char* WordReader::readLine()
{
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return NULL;
	}

	// Skip leading spaces, if any.
	/*
	while(isspace(bufPtr[readPos]))
	{
		++readPos;
	}
	*/


	const char *ptr = bufPtr+readPos;

	// Move readPos to next sub string.
	while(isprint(bufPtr[readPos]))
	{
		++readPos;
	}

	bufPtr[readPos]=0;

	++readPos;

	return ptr;
}

// end of stream
bool WordReader::endOfStr()
{
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return true;
	}
	return false;
}

// Read a string, a string is a number of words surrounded by quotes '"'.
// NOTE! If the string needs to be saved it must be copied. This will return a pointer to an internal memory that will be reused by the time of next call to WordReader.
const char* WordReader::readString()
{
	int state=0;
	char quoteChar='"';


	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return NULL;
	}


	char* beginPtr = bufPtr+readPos;

	// Skip leading spaces, if any.
	while(isspace(*beginPtr))
	{
		++beginPtr;
	}


	// Move readPos to next sub string.
	/*while(isprint(bufPtr[readPos]))
	{
		++readPos;
	}*/

	const char* readPtr = beginPtr;
	char* writePtr = beginPtr;


	while (state<4)
	{
		const int ch = *readPtr;
		++readPtr;

		// Check for end of file or closed socket
		if (ch==0)
		{
			break;
		}

		switch(state)
		{
		     case 0:
		     {
		 		// skip spaces, look for leading '"'
		 		if (isprint(ch))
		 		{
			        // First char in a string shall be a '"'
			 		if (ch=='"')
					{
			 			// It was
			 			state=1;
					}
			 		else if (ch=='\'')
					{
			 			// ok, will allow single quotes also
			 			quoteChar='\'';
			 			state=1;
					}
			 		else
			 		{
			 			// This string did not begin with quotes, so it will end with space instead
						//debug("String shall start with '\"'");
			 			*writePtr = ch;
			 			++writePtr;
						state=3;
			 		}
		 		}
		 		break;
		     }
		     case 1:
		     {
		    	// Now look for the trailing '"' or the escape char '\'
				if (ch==quoteChar)
				{
					// This marks the end of the string

					// read/skip trailing space zero etc, if any.
					while (!isprint(*readPtr))
					{
						++readPtr;
					}

					state=4;
				}
				else if (ch=='\\')
				{
					// This is the escape char, special char follows-
					state=2;
				}
				else
				{
					// This was part of the string
		 			*writePtr = ch;
		 			++writePtr;
				}
				break;
		     }
		     case 2:
		     {
		    	// https://en.wikipedia.org/wiki/Escape_sequences_in_C
		    	switch(ch)
		    	{
	    			case 'a':
						*writePtr = '\a';
						break;
	    			case 'b':
						*writePtr = '\b';
						break;
	    			case 'f':
						*writePtr = '\f';
						break;
		    		case 'n':
						*writePtr = '\n';
						break;
		    		case 'r':
						*writePtr = '\r';
						break;
		    		case 't':
						*writePtr = '\t';
						break;
		    		case 'u':
		    		case 'U':
						printf("unicode escape sequence is not supported yet");
						break;
		    		case 'v':
						*writePtr = '\v';
						break;
	    			case 'x':
						printf("hex escape sequence is not supported yet");
						break;
		    		case '\\':
		    		case '\'':
		    		case '\"':
		    		case '?':
						*writePtr = ch;
						break;
		    		default:
					{
						if ((ch>='0') && (ch<='9'))
						{
							printf("octal escape sequence is not supported yet");
						}
						else
						{
							printf("incorrect escape sequence %d", ch);
						}
				    	*writePtr = ch;
						break;
					}
		    	}

	 			++writePtr;
		    	state=1;
				break;
		     }
		     case 3:
		     {
		    	// Now look for the trailing space
			 	if (!isgraph(ch))
			 	{
			 		state=4;
			 	}
				else
				{
		 			*writePtr = ch;
		 			++writePtr;
				}
				break;
		     }
		} // switch(state)

	} // while (state<4)


	// write the string terminating zero
	*writePtr = 0;
	++writePtr;


	readPos = readPtr - bufPtr;


	return beginPtr;
}



// Gives true if next word to read matches str
bool WordReader::equalsIgnoreCase(const char *str)
{
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return str==NULL;
	}

	const char *ptr = bufPtr+readPos;

	const size_t len=strlen(str);

	if (strncmp(ptr, str, len)==0)
	{
		if (!isgraph(ptr[len]))
		{
			return true;
		}
	}

	return false;
}


int WordReader::readInt()
{
	const char *s=readWord();
	if (s==NULL)
	{
		printf("WordReader::readFloat: NULL pointer\n");
		return 0;
	}
	// TODO: check that it really is an integer.
	return atoi(s);
}

long long WordReader::readLong()
{
	const char *s=readWord();
	if (s==NULL)
	{
		printf("WordReader::readFloat: NULL pointer\n");
		return 0;
	}
	// TODO: check that it really is an integer.
	return strtoll(s, NULL, 0); // Warning: a string like 010 will be parsed as octal so it gives 8 not 10.
}


float WordReader::readFloat()
{
	const char *s=readWord();
	if (s==NULL)
	{
		printf("WordReader::readFloat: NULL pointer\n");
		return 0.0f;
	}
	return atof(s);
}

// http://stackoverflow.com/questions/1012571/stdstring-to-float-or-double
/*
double WordReader::readDouble()
{
	const char *s=getNextString();
	return std::stod(s);
}
*/



bool WordReader::readBoolean()
{
	const char *s=readWord();
	if (s==NULL)
	{
		printf("WordReader::readBoolean: NULL pointer\n");
		return 0;
	}

	if (strcmp(s,"0")==0)
	{
		return false;
	}
	else if (strcmp(s,"1")==0)
	{
		return true;
	}
	else
	{
		printf("WordReader::readBoolean: expected 0 or 1\n");
		return atoi(s);
	}
}


