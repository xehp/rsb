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
#include <inttypes.h>
#include "WordReader.h"



int wordReaderDebugCounter=0;
int wordReaderLinkDebugCounter=0;

// Note n is number of bytes not characters if string is wchar_t
WordReaderLink::WordReaderLink(const unsigned int n, WordReaderLink *first):
	next(first)
{
	bufPtr = (char*)malloc(n+1); // +1 to make room for a terminating zero
	//bufPtr = (char*)malloc(4*n+8); // TODO There is a buffer overrun somewhere... Needed this to try to locate it.
	bufPtr[n]=0; // write the terminating zero right away, need this to be able to detect buffer overwriting in destructor.
	bufSizeDebug=n;
	wordReaderLinkDebugCounter++;
}

WordReaderLink::~WordReaderLink()
{
	assert(bufPtr[bufSizeDebug]==0); // detect buffer overwriting
	free(bufPtr);
	next=NULL;
	wordReaderLinkDebugCounter--;
}


void WordReaderLink::cleanup()
{
	assert(wordReaderLinkDebugCounter==0);
}

void WordReaderLink::debugAssert()
{
	/*if (bufPtr[bufSizeDebug]!=0)
	{
		printf("why\n");
	}*/
	assert(bufPtr[bufSizeDebug]==0); // detect buffer overwriting
}



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
	bufPtr(str),
	//bufSize(0),
	//nBytes(0),
	//nWords(0),
	readPos(0),
	first(NULL),
	separatorChar(0)
{
	const unsigned int strLen=strlen(str);
	bufSize = strLen;
	assert(bufPtr!=NULL);
	//skipSpaces();
	wordReaderDebugCounter++;
}

WordReader::~WordReader()
{
	while(first!=NULL)
	{
		WordReaderLink *tmp=first;
		first=first->next;
		delete tmp;
	}
	wordReaderDebugCounter--;
}


void WordReader::skipSpaces()
{
	// Skip leading spaces, if any.
	while(isspace(bufPtr[readPos]))
	{
		++readPos;
	}
}

// Skip n bytes and skip trailing spaces including one separator.
void WordReader::skipNSpacesSeparator(const int n)
{
	readPos +=n;

	// Skip leading spaces, if any.
	while(isspace(bufPtr[readPos]))
	{
		++readPos;
	}
	if (separatorChar)
	{
		if (bufPtr[readPos]==separatorChar)
		{
			++readPos;
			while(isspace(bufPtr[readPos]))
			{
				++readPos;
			}
		}
	}
	assert(readPos<=bufSize);
}


// Look at what is left in the buffer without reading it. Mostly used for debugging.
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

// Read a word, a word is a number of characters separated by space (or line feed etc), typically it is a number actually since text is sent in strings.
// NOTE! If the string needs to be saved it must be copied. This will return a pointer to an internal memory that will be reused by the time of next call to WordReader.
// deprecated, use return readToken instead.
const char* WordReader::readWord()
{
	skipSpaces();
	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return NULL;
	}


	// Remember where the word begins
	const char *beginPtr = bufPtr+readPos;

	// How long is it?
	int n=0;
	while(isgraph(beginPtr[n]))
	{
		++n;
	}

	if (n>0)
	{
		WordReaderLink *tmp = new WordReaderLink(n, first);
		first=tmp;
		memcpy(tmp->bufPtr, beginPtr, n);
		tmp->bufPtr[n]=0;
		readPos+=n;

		//skipSpaces();
		return first->bufPtr;
	}

	return NULL;
}

int WordReader::readWord(char* strPtr, int strSize)
{
	const char *tmpPtr;
	int n = readToken(&tmpPtr);
	if (n<=0)
	{
		// No more data to read
		return 0;
	}
	if (n>=strSize)
	{
		// Next token did not fit in given buffer
		return -1;
	}
	memcpy(strPtr, tmpPtr, n);
	strPtr[n]=0;
	//skipSpaces();
	return n;
}

static bool my_isprint(unsigned int ch)
{
	return isprint(ch) || (ch>=160);
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

	// Remember where the word begins
	const char *beginPtr = bufPtr+readPos;

	// How long is it?
	int n=0;
	while(my_isprint(beginPtr[n]))
	//while(isprint(beginPtr[n]))
	{
		++n;
	}

	if (n>0)
	{
		WordReaderLink *tmp = new WordReaderLink(n+1, first);
		first=tmp;

		//memcpy(tmp->bufPtr, beginPtr, n);
		//tmp->bufPtr[n]=0;
		int m=decodeString(tmp->bufPtr, beginPtr, n);
		assert(m<=n);

		// WordReaderLink does allocate one byte extra for the terminating zero so this is OK
		tmp->bufPtr[m]=0;

		readPos+=n;

		//skipSpaces();
		// Or skip all except spaces
		//while(!isgraph(bufPtr[readPos]))
		// Just skip the end of line marker.
		if ((readPos<bufSize) && (bufPtr[readPos]=='\n'))
		{
			++readPos;
		}

		return first->bufPtr;
	}

	return NULL;
}

// Same as readWord but reads lines
// NOTE! If the string needs to be saved it must be copied. This will return a pointer to an internal memory that will be reused by the time of next call to WordReader.
const wchar_t* WordReader::readLineW()
{

	if(readPos>=bufSize)
	{
		// We have no more sub strings.
		return NULL;
	}

	// Remember where the word begins
	const char *beginPtr = bufPtr+readPos;

	// How long is it?
	int n=0;
	while(my_isprint(beginPtr[n]))
	//while(isprint(beginPtr[n]))
	{
		++n;
	}

	if (n>0)
	{
		WordReaderLink *tmp = new WordReaderLink((n+1)*sizeof(wchar_t), first);
		first=tmp;

		wchar_t* ptr = (wchar_t*)tmp->bufPtr;
		//memcpy(tmp->bufPtr, beginPtr, n);
		//tmp->bufPtr[n]=0;
		int m=decodeStringW(ptr, beginPtr, n);
		assert(m<=n);

		// WordReaderLink does allocate one byte extra for the terminating zero so this is OK
		ptr[m]=0;

		readPos+=n;

		//skipSpaces();
		// Or skip all except spaces
		//while(!isgraph(bufPtr[readPos]))
		// Just skip the end of line marker.
		if ((readPos<bufSize) && (bufPtr[readPos]=='\n'))
		{
			++readPos;
		}

		return (wchar_t*)first->bufPtr;
	}

	return NULL;
}


// Gives true if at end of stream/buffer (or there are only spaces and such left)
bool WordReader::endOfStr() const
{
	int tmpPos=readPos;
	while(isspace(bufPtr[tmpPos]))
	{
		++tmpPos;
	}
	if(tmpPos>=bufSize)
	{
		// We have no more sub strings.
		return true;
	}
	return false;
}

// Read a string, a string is a number of words surrounded by quotes '"'.
// NOTE! If the string needs to be saved it must be copied. This will return a pointer to an internal memory that will be reused by the time of next call to WordReader.
// deprecated, use return readToken instead.
const char* WordReader::readString()
{
	return readToken();
}


// Deprecated, use isNext instead.
// Gives true if next word to read matches str
// Does this really ignore case?
bool WordReader::equalsIgnoreCase(const char *str) const
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

// Gives true if next token matches str.
bool WordReader::isNext(const char *str) const
{
	TokenType tokenType;
	const char *tmpPtr;

	const int n=examineToken(&tmpPtr, &tokenType);

	if (str==NULL)
	{
		return (n<0);
	}

	const size_t len=strlen(str);

	if (len!=n)
	{
		return false;
	}

	if (strncmp(tmpPtr, str, len)==0)
	{
		return true;
	}

	return false;
}


int WordReader::readInt()
{
	TokenType tokenType;

	const char *tmpPtr;
	int n = readToken(&tmpPtr, &tokenType);
	if (n<=0)
	{
		// No more data to read
		printf("WordReader::readInt: number expected but not found\n");
		fflush(stdout);
		return 0;
	}
	if (tokenType!=NumberToken)
	{
		// It was not a number
		printf("WordReader::readInt: number expected but not found %d '%.32s'\n", (int)tokenType, tmpPtr);
		fflush(stdout);
		return 0;
	}

	return atoi(tmpPtr);
}

long long WordReader::readLong()
{
	TokenType tokenType;

	const char *tmpPtr;
	int n = readToken(&tmpPtr, &tokenType);
	if (n<=0)
	{
		// No more data to read
		printf("WordReader::readLong: number expected but not found\n");
		fflush(stdout);
		return 0;
	}
	if (tokenType!=NumberToken)
	{
		// It was not a number
		printf("WordReader::readLong: number expected but not found %d '%.32s'\n", (int)tokenType, tmpPtr);
		fflush(stdout);
		return 0;
	}

	return strtoll(tmpPtr, NULL, 0); // Warning: a string like 010 will be parsed as octal so it gives 8 not 10.
}


float WordReader::readFloat()
{
	TokenType tokenType;

	const char *tmpPtr;
	int n = readToken(&tmpPtr, &tokenType);
	if (n<=0)
	{
		// No more data to read
		printf("WordReader::readFloat: number expected but not found\n");
		fflush(stdout);
		return 0;
	}
	if (tokenType!=NumberToken)
	{
		// It was not a number
		printf("WordReader::readFloat: number expected but not found %d '%.32s'\n", (int)tokenType, tmpPtr);
		fflush(stdout);
		return 0;
	}

	return atof(tmpPtr);
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
	char tmpStr[80];
	const int n=readWord(tmpStr, sizeof(tmpStr));

	if (n<=0)
	{
		printf("WordReader::readBoolean: bool expected but not found\n");
		fflush(stdout);
		return 0;
	}

	if (strcmp(tmpStr,"0")==0)
	{
		return false;
	}
	else if (strcmp(tmpStr,"1")==0)
	{
		return true;
	}
	else if (strcmp(tmpStr,"false")==0)
	{
		return false;
	}
	else if (strcmp(tmpStr,"true")==0)
	{
		return true;
	}
	else
	{
		printf("WordReader::readBoolean: expected boolean but found '%.32s'\n", tmpStr);
		fflush(stdout);
		return atoi(tmpStr);
	}
}


static bool isInternalPartOfNumber(char ch)
{
	if ((ch>='0' && ch<='9'))
	{
		return true;
	}
	else if (((ch=='.') || (ch=='-') || (ch=='E') || (ch=='e')))
	{
		// OK, will accept parts of a float also.
		// Floats can look like this: 9.223372E12 or -2.398081733190338e-14
		// This code does not check that there is only one E and one '.'.
		return true;
	}
	return false;
}


// This examines what is next to read
// A pointer to the string is given in tokenPtr.
// NOTE that the string is not zero terminated.
// If a zero terminated string is needed it must be copied over to another buffer (that is length +1 in size).
// It returns the length of the string.
int WordReader::examineToken(const char **tokenPtr, TokenType *tokenType) const
{
	const char *singleCharTokens=",.;[](){}";
	const char *multiCharTokens="~^&|<>:=!*/%?+-";
	ReadTokenState state=InitialState;
	int stateCounter=0;
	char quoteChar='"';

	if(readPos>=bufSize)
	{
		*tokenPtr=NULL;
		return 0;
	}


	const char* beginPtr = bufPtr+readPos;
	const char* readPtr = beginPtr;
	*tokenPtr = beginPtr;


	while (state!=EndState)
	{
		const int ch = *readPtr;

		// Check for end of file or closed socket (actually just end of string in this case)
		if (ch==0)
		{
			break;
		}


		switch(state)
		{
			case InitialState: // initial state, skipping spaces and separators
			{
				if (isdigit(ch))
				{
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=NumberToken;
					}
					state=ParsingNumber;
				}
				else if (isalnum(ch) || (ch=='_'))
				{
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=NameToken;
					}
					state=ParsingName;
				}
				else if (ch=='-')
				{
					readPtr++;
					state=NumberOrOpcode;
				}
				else if (ch=='"')
				{
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=SimpleString;
					}
					state=InsideStringNoEscSeq;
				}
				else if (ch=='\'')
				{
					// single quoted string
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=SimpleString;
					}
					quoteChar=ch;
					state=InsideStringNoEscSeq;
				}
				else if (strchr(singleCharTokens, ch)!=NULL)
				{
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=Opcode;
					}
					state=EndState;
				}
				else if (strchr(multiCharTokens, ch)!=NULL)
				{
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=Opcode;
					}
					state=ParsingOpcode;
				}
				else
				{
					// all others treated as single char unknown tokens
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=UnknowToken;
					}
					state=EndState;
				}
				break;
			}
			case ParsingNumber: // a number,
			{
				// Now look for the trailing space
				if (!isInternalPartOfNumber(ch))
				{
					assert((tokenType==NULL) || (*tokenType==NumberToken));
					state=EndState;
				}
				else
				{
					readPtr++;
				}
				break;
			}
			case ParsingName: // a normal word or name,
			{
				// Now look for the trailing space
				if (!isalnum(ch) && (ch!='_'))
				{
					assert((tokenType==NULL) || (*tokenType==NameToken));
					state=EndState;
				}
				else
				{
					readPtr++;
				}
				break;
			}
			case NumberOrOpcode: // a number or an opcode?
			{
				if (isdigit(ch))
				{
					// Part of a number, as in -9.01
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=NumberToken;
					}
					state=ParsingNumber;
				}
				else if (strchr(multiCharTokens, ch)!=NULL)
				{
					// Part of an opcode such as --
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=Opcode;
					}
					state=ParsingOpcode;
				}
				else
				{
					if (tokenType!=NULL)
					{
						*tokenType=Opcode;
					}
					state=EndState;
				}
				break;
			}
			case ParsingOpcode: // part of an multi char opcode
			{
				if (strchr(multiCharTokens, ch)==NULL)
				{
					assert((tokenType==NULL) || (*tokenType==Opcode));
					state=EndState;
				}
				else
				{
					readPtr++;
				}
				break;
			}

			case InsideStringNoEscSeq:
			{
				// Now look for the trailing '"' or the escape char '\'
				if (ch==quoteChar)
				{
					// This marks the end of the string (and is part of it)
					readPtr++;
					assert((tokenType==NULL) || (*tokenType==SimpleString));
					state=EndState;
				}
				else if (ch=='\\')
				{
					// This is the escape char, special char follows-
					readPtr++;
					if (tokenType!=NULL)
					{
						*tokenType=StringWithEscSeq;
					}
					state=EscapeCharSeq;
				}
				else
				{
					// This is part of the string.
					readPtr++;
				}
				break;
			}
			case EscapeCharSeq:
			{
				// https://en.wikipedia.org/wiki/Escape_sequences_in_C
		    	switch(ch)
		    	{
	    			case 'a':
						++readPtr;
						break;
	    			case 'b':
						++readPtr;
						break;
	    			case 'f':
						++readPtr;
						break;
		    		case 'n':
						++readPtr;
						break;
		    		case 'r':
						++readPtr;
						break;
		    		case 't':
						++readPtr;
						break;
		    		case 'u':
		    			// Universal character names, \uxxxx
						++readPtr;
						state=AfterEscapeCharSeq;
						stateCounter=4;
		    		case 'U':
		    			// Universal character names, \Uxxxxxxxx
						++readPtr;
						state=AfterEscapeCharSeq;
						stateCounter=8;
						break;
		    		case 'v':
						++readPtr;
						break;
	    			case 'x':
						// The character whose numerical value is given by hh interpreted as a hexadecimal number
						++readPtr;
						state=AfterEscapeCharSeq;
						stateCounter=2;
						break;
		    		case '\\':
		    		case '\'':
		    		case '\"':
		    		case '?':
						++readPtr;
						break;
		    		default:
					{
						if ((ch>='0') && (ch<='7'))
						{
							// The character whose numerical value is given by nnn interpreted as an octal number
							++readPtr;
			    			// TODO check that 3 more octal digits follow
							readPtr+=3;
						}
						else
						{
							++readPtr;
							printf("incorrect escape sequence %d", ch);
						}
						break;
					}
		    	}

				state=InsideStringWithEscSeq;
				break;
			 }
			case AfterEscapeCharSeq:
			{
				--stateCounter;
				if (stateCounter<=0)
				{
					state=InsideStringWithEscSeq;
				}
				break;
			}
			case InsideStringWithEscSeq:
			{
				// Now look for the trailing '"' or the escape char '\'
				if (ch==quoteChar)
				{
					// This marks the end of the string
					++readPtr;
					assert((tokenType==NULL) || (*tokenType==StringWithEscSeq));
					state=EndState;
				}
				else if (ch=='\\')
				{
					// This is the escape char, special char follows-
					++readPtr;
					state=EscapeCharSeq;
				}
				else
				{
					// This is part of the string.
					++readPtr;
				}
				break;
			}

			default:
			{
				break;
			}
		}
	}

	return readPtr - beginPtr;
}

int WordReader::readToken(const char **tokenPtr, TokenType *tokenType)
{
	skipSpaces();
	const int n=examineToken(tokenPtr, tokenType);
	skipNSpacesSeparator(n);
	return n;
}

// This reads the next work/token/string from internal buffer or stream.
// NOTE! If the string needs to be saved (beyond the life of the WordReader) it must be copied by caller. This will return a pointer to an internal memory that will be freed when the WordReader is deleted.
// If the string is only needed while WordReader still exist then it does not need to be copied.
const char *WordReader::readToken()
{
	TokenType escSeq(UnknowToken);
	const char *str;
	int n = readToken(&str, &escSeq);
	if (n>0)
	{
		WordReaderLink *tmp = new WordReaderLink(n, first);
		first=tmp;
		char *writePtr=tmp->bufPtr;

		switch(escSeq)
		{
		default:
		case NumberToken:
		case NameToken:
		case Opcode:
			memcpy(writePtr, str, n);
			writePtr[n]=0;
			break;
		case SimpleString:
			{
				assert(n>=2);
				assert((str[n-1]=='"') || (str[n-1]=='\''));
				str++;
				n-=2;

				memcpy(writePtr, str, n);
				writePtr[n]=0;
				break;
			}
		case StringWithEscSeq:
			{
				assert(n>=2);
				assert((str[n-1]=='"') || (str[n-1]=='\''));
				str++;
				n-=2;

				int state=0;
				int digitsExpected=0;
				int digitsFound=0;
				int someDigits=0;
				for(int i=0; i<n; ++i)
				{
					const char ch = str[i];
					switch(state)
					{
					default:
					case 0:
						if (ch=='\\')
						{
							state=1;
						}
						else
						{
							*writePtr++=str[i];
						}
						break;
					case 1:
						{
							// https://en.wikipedia.org/wiki/Escape_sequences_in_C
							switch(ch)
							{
								case 'a':
									*writePtr++ = '\a';
									state=0;
									break;
								case 'b':
									*writePtr++ = '\b';
									state=0;
									break;
								case 'f':
									*writePtr++ = '\f';
									state=0;
									break;
								case 'n':
									*writePtr++ = '\n';
									state=0;
									break;
								case 'r':
									*writePtr++ = '\r';
									state=0;
									break;
								case 't':
									*writePtr++ = '\t';
									state=0;
									break;
								case 'u':
									*writePtr++ = '\\';
									*writePtr++ = ch;
									state=0;
									break;
								case 'U':
									*writePtr++ = '\\';
									*writePtr++ = ch;
									state=0;
									break;
								case 'v':
									*writePtr++ = '\v';
									state=0;
									break;
								case 'x':
									digitsFound=0;
									digitsExpected=2;
									someDigits=0;
									state=2;
									break;
								case '\\':
								case '\'':
								case '\"':
								case '?':
									*writePtr++ = ch;
									state=0;
									break;
								default:
								{
									if ((ch>='0') && (ch<='7'))
									{
										digitsFound=1;
										digitsExpected=3;
										someDigits=ch-'0';
										state=3;
									}
									else
									{
										*writePtr++ = '\\';
										*writePtr++ = ch;
										state=0;
									}
									break;
								}
							}

							break;
						 }
					case 2:
						{

							if ((ch>='0') && (ch<='9'))
							{
								someDigits=(someDigits<<4)+ch-'0';
							}
							else if ((ch>='A') && (ch<='F'))
							{
								someDigits=(someDigits<<4)+ch-'A'+10;
							}
							else if ((ch>='a') && (ch<='f'))
							{
								someDigits=(someDigits<<4)+ch-'a'+10;
							}
							else
							{
								*writePtr++ = someDigits;
								*writePtr++ = ch;
								state=0;
							}
							++digitsFound;
							if (digitsFound==digitsExpected)
							{
								*writePtr++ = someDigits;
								state=0;
							}
							break;
						}
					case 3:
						{

							if ((ch>='0') && (ch<='7'))
							{
								someDigits=(someDigits<<3)+ch-'0';
							}
							else
							{
								*writePtr++ = someDigits;
								*writePtr++ = ch;
								state=0;
							}
							++digitsFound;
							if (digitsFound==digitsExpected)
							{
								*writePtr++ = someDigits;
								state=0;
							}
							break;
						}
					}
				}
				*writePtr=0;
			}
			break;
		}

		if ((char*)writePtr>=(tmp->bufPtr+tmp->bufSizeDebug))
		{
			printf("but why though\n");
		}

		assert(writePtr<(tmp->bufPtr+tmp->bufSizeDebug));
		tmp->debugAssert();
		return first->bufPtr;
	}
	return NULL;
}

// Copy byte string to wide string
// n is number of characters to copy,
// dst must point to a buffer with room for n wchar_t characters (not just bytes)
// The trailing zero is not copied.
void WordReader::cpyW(wchar_t *dst, const char* src, size_t n)
{
	while(n>0)
	{
		*dst=*src;
		++dst;
		++src;
		--n;
	}
}


// This reads the next work/token/string from internal buffer or stream.
// NOTE! If the string needs to be saved (beyond the life of the WordReader) it must be copied by caller. This will return a pointer to an internal memory that will be freed when the WordReader is deleted.
// If the string is only needed while WordReader still exist then it does not need to be copied.
const wchar_t *WordReader::readTokenW()
{
	TokenType escSeq(UnknowToken);
	const char *str;
	const int n = readToken(&str, &escSeq);
	if (n>0)
	{
		WordReaderLink *tmp = new WordReaderLink((n+1)*sizeof(wchar_t), first);
		first=tmp;
		wchar_t *writePtr=(wchar_t*)tmp->bufPtr;

		switch(escSeq)
		{
		default:
		case NumberToken:
		case NameToken:
		case Opcode:
			cpyW(writePtr, str, n);
			writePtr[n]=0;
			break;
		case SimpleString:
			{
				assert(n>=2);
				assert((str[n-1]=='"') || (str[n-1]=='\''));
				str++;

				cpyW(writePtr, str, n-2);
				writePtr[n-2]=0;
				break;
			}
		case StringWithEscSeq:
			{
				assert(n>=2);
				assert((str[n-1]=='"') || (str[n-1]=='\''));
				str++;

				int state=0;
				int digitsExpected=0;
				int digitsFound=0;
				int someDigits=0;
				for(int i=0; i<n; ++i)
				{
					const char ch = str[i];
					switch(state)
					{
					default:
					case 0:
						if (ch=='\\')
						{
							state=1;
						}
						else
						{
							*writePtr++=str[i];
						}
						break;
					case 1:
						{
							// https://en.wikipedia.org/wiki/Escape_sequences_in_C
							switch(ch)
							{
								case 'a':
									*writePtr++ = '\a';
									state=0;
									break;
								case 'b':
									*writePtr++ = '\b';
									state=0;
									break;
								case 'f':
									*writePtr++ = '\f';
									state=0;
									break;
								case 'n':
									*writePtr++ = '\n';
									state=0;
									break;
								case 'r':
									*writePtr++ = '\r';
									state=0;
									break;
								case 't':
									*writePtr++ = '\t';
									state=0;
									break;
								case 'u':
									*writePtr++ = '\\';
									*writePtr++ = ch;
									state=0;
									break;
								case 'U':
									*writePtr++ = '\\';
									*writePtr++ = ch;
									state=0;
									break;
								case 'v':
									*writePtr++ = '\v';
									state=0;
									break;
								case 'x':
									digitsFound=0;
									digitsExpected=2;
									someDigits=0;
									state=2;
									break;
								case '\\':
								case '\'':
								case '\"':
								case '?':
									*writePtr++ = ch;
									state=0;
									break;
								default:
								{
									if ((ch>='0') && (ch<='7'))
									{
										digitsFound=1;
										digitsExpected=3;
										someDigits=(someDigits<<3)+ch-'0';
										state=3;
									}
									else
									{
										*writePtr++ = '\\';
										*writePtr++ = ch;
										state=0;
									}
									break;
								}
							}

							break;
						 }
					case 2:
						{

							if ((ch>='0') && (ch<='9'))
							{
								someDigits=(someDigits<<4)+ch-'0';
							}
							else if ((ch>='A') && (ch<='F'))
							{
								someDigits=(someDigits<<4)+ch-'A'+10;
							}
							else if ((ch>='a') && (ch<='f'))
							{
								someDigits=(someDigits<<4)+ch-'a'+10;
							}
							else
							{
								*writePtr++ = someDigits;
								*writePtr++ = ch;
								state=0;
							}
							++digitsFound;
							if (digitsFound==digitsExpected)
							{
								*writePtr++ = someDigits;
								state=0;
							}
							break;
						}
					case 3:
						{

							if ((ch>='0') && (ch<='7'))
							{
								someDigits=(someDigits<<3)+ch-'0';
							}
							else
							{
								*writePtr++ = someDigits;
								*writePtr++ = ch;
								state=0;
							}
							++digitsFound;
							if (digitsFound==digitsExpected)
							{
								*writePtr++ = someDigits;
								state=0;
							}
							break;
						}
					}
				}
				*writePtr=0;
			}
			break;
		}

		/*if ((char*)writePtr>=(tmp->bufPtr+tmp->bufSizeDebug))
		{
			printf("but why though %ls %d\n",(wchar_t*)tmp->bufPtr, tmp->bufSizeDebug);
		}*/

		assert((char*)writePtr<(tmp->bufPtr+tmp->bufSizeDebug));
		tmp->debugAssert();

		return (wchar_t*)tmp->bufPtr;
	}
	return NULL;
}

#ifdef JSON_READER

// Returns true is next word/token read matched expectedStr.
bool WordReader::readExpected(const char *expectedStr)
{
	TokenType tokenType;
	const char *tmpPtr;

	skipSpaces();
	const int n=examineToken(&tmpPtr, &tokenType);
	skipNSpacesSeparator(n);

	if (expectedStr==NULL)
	{
		if (n>0)
		{
			printf("WordReader::readExpected: Expected end but found '%.32s'\n", tmpPtr);
			fflush(stdout);
			return false;
		}
		return true;
	}

	const size_t len=strlen(expectedStr);


	if ((len==n) && (strncmp(tmpPtr, expectedStr, len)==0))
	{
		return true;
	}

	printf("WordReader::readExpected: Did not find '%.32s', found '%.32s'\n", expectedStr, tmpPtr);
	fflush(stdout);

	return false;
}

#endif

// See also encodeHex in WordWriter
static int decodeHex(int ch)
{
	if ((ch>='0') && (ch<='9'))
	{
		return ch-'0';
	}
	if ((ch>='A') && (ch<='F'))
	{
		return ch-'A'+10;
	}
	if ((ch>='a') && (ch<='f'))
	{
		return ch-'a'+10;
	}
	return -1;
}



// Returns number of characters written to dst, not including terminating zero.
// See also encodeString in WordWriter
// NOTE the string will not always be null terminated after this
int WordReader::decodeString(char *dst, const char *src, int n)
{
	int state=0;
	int stateCounter=0;
	int composedChar=0;
	char *orgDst=dst;
	while((*src) && (n>0))
	{
		const int ch = *src;
		switch(state)
		{
		case 0:
			switch(ch)
			{
			case '\\':
				state=4;
				break;
			case '&':
				state=1;
				break;
			default:
				assert(n>0);
				--n;
				*dst=ch;
				++dst;
				break;
			}
			break;
		case 1:
			if (ch=='#')
			{
				composedChar=0;
				stateCounter=2;
				state=5;
			}
			else
			{
				assert(n>=2);
				n-=2;
				*dst='&';
				++dst;
				*dst=ch;
				++dst;
				state=0;
			}
			break;
		case 4:
			switch(ch)
			{
			case '\\':
				assert(n>0);
				--n;
				*dst='\\';
				++dst;
				state=0;
				break;
			case '\'':
				assert(n>0);
				--n;
				*dst='\'';
				++dst;
				state=0;
				break;
			case '\"':
				assert(n>0);
				--n;
				*dst='\"';
				++dst;
				state=0;
				break;
			case 'x':
				composedChar=0;
				stateCounter=2;
				state=5;
				break;
			case 'u':
				composedChar=0;
				stateCounter=4;
				state=5;
				break;
			case 'U':
				composedChar=0;
				stateCounter=8;
				state=5;
				break;
			case 'n':
				assert(n>0);
				--n;
				*dst='\n';
				++dst;
				state=0;
				break;
			case 'r':
				assert(n>0);
				--n;
				*dst='\r';
				++dst;
				state=0;
				break;
			case 't':
				assert(n>0);
				--n;
				*dst='\t';
				++dst;
				state=0;
				break;
			default:
				assert(n>=2);
				n-=2;
				*dst='\\';
				++dst;
				*dst=ch;
				++dst;
				state=0;
				break;
			}
			break;
		case 5:
			composedChar=(composedChar<<4) + decodeHex(ch);
			--stateCounter;
			if (stateCounter<=0)
			{
				assert(n>0);
				--n;
				*dst=composedChar;
				++dst;
				state=0;
			}
			break;

		default:
			break;
		}
		++src;
	}
	if (n>0)
	{
		*dst=0;
	}
	return dst-orgDst;
}

// Returns number of characters written to dst, not including terminating zero.
// See also encodeString in WordWriter
// NOTE the string will not always be null terminated after this
int WordReader::decodeStringW(wchar_t *dst, const char *src, int n)
{
	int state=0;
	int stateCounter=0;
	unsigned int composedChar=0;
	wchar_t *orgDst=dst;
	while((*src) && (n>0))
	{
		const unsigned char ch = *src;
		switch(state)
		{
		case 0:
			switch(ch)
			{
			case '\\':
				state=4;
				break;
			/*case '&':
				state=1;
				break;*/
			default:
				assert(n>0);
				--n;
				*dst=ch;
				++dst;
				break;
			}
			break;
		/*case 1:
			if (ch=='#')
			{
				composedChar=0;
				stateCounter=2;
				state=2; // It shall read hex until it finds ';'
			}
			else
			{
				assert(n>=2);
				n-=2;
				*dst='&';
				++dst;
				*dst=ch;
				++dst;
				state=0;
			}
			break;*/
		case 4:
			switch(ch)
			{
			case '\\':
				assert(n>0);
				--n;
				*dst='\\';
				++dst;
				state=0;
				break;
			case '\'':
				assert(n>0);
				--n;
				*dst='\'';
				++dst;
				state=0;
				break;
			case '\"':
				assert(n>0);
				--n;
				*dst='\"';
				++dst;
				state=0;
				break;
			case 'x':
				composedChar=0;
				stateCounter=2;
				state=5;
				break;
			case 'u':
				composedChar=0;
				stateCounter=4;
				state=5;
				break;
			case 'U':
				composedChar=0;
				stateCounter=8;
				state=5;
				break;
			case 'n':
				assert(n>0);
				--n;
				*dst='\n';
				++dst;
				state=0;
				break;
			case 'r':
				assert(n>0);
				--n;
				*dst='\r';
				++dst;
				state=0;
				break;
			case 't':
				assert(n>0);
				--n;
				*dst='\t';
				++dst;
				state=0;
				break;
			default:
				assert(n>=2);
				n-=2;
				*dst='\\';
				++dst;
				*dst=ch;
				++dst;
				state=0;
				break;
			}
			break;
		case 5:
			composedChar=(composedChar<<4) + decodeHex(ch);
			--stateCounter;
			if (stateCounter<=0)
			{
				assert(n>0);
				--n;
				*dst=composedChar;
				++dst;
				state=0;
			}
			break;

		default:
			break;
		}
		++src;
	}
	if (n>0)
	{
		*dst=0;
	}
	return dst-orgDst;
}


void WordReader::cleanup()
{
	assert(wordReaderDebugCounter==0);
	WordReaderLink::cleanup();
}



// 00000000 - 0000007F 	0xxxxxxx
// 00000080 - 000007FF 	110xxxxx 10xxxxxx
// 00000800 - 0000FFFF 	1110xxxx 10xxxxxx 10xxxxxx
// 00010000 - 001FFFFF 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx


// Begin hoehrmann UTF8 decode
// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

static const uint8_t utf8d[] = {
  // The first part of the table maps bytes to character classes that
  // to reduce the size of the transition table and create bitmasks.
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

  // The second part is a transition table that maps a combination
  // of a state of the automaton and a character class to a state.
   0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12,
};

uint32_t inline
decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
  uint32_t type = utf8d[byte];

  *codep = (*state != UTF8_ACCEPT) ?
    (byte & 0x3fu) | (*codep << 6) :
    (0xff >> type) & (byte);

  *state = utf8d[256 + *state + type];
  return *state;
}
// End hoehrmann UTF8 decode



// Decode utf-8
// n shall indicate the size of array dst
// trailing zero is written if there is room for it.
// returns number of characters if OK, a negative number if not OK.
int WordReader::decodeUtf8(wchar_t *dst, const char *src, int nDst)
{
	int n=0;

	uint32_t codepoint;
	uint32_t state = 0;
	while ((*src) && (n<nDst))
	{
		unsigned char ch=*src;
	    if (!decode(&state, &codepoint, ch))
	    {
	    	*dst=codepoint;
	    	++dst;
   	    	++n;
	    }
		++src;
	}

	if (n<nDst)
	{
		*dst=0;
	}

    return (state != UTF8_ACCEPT)?-1:n;
}


// Count number of characters (wchar_t) in an utf8 encoded buffer.
// returns number of characters if OK, a negative number if not OK.
int WordReader::countUtf8(const char *src)
{
	int n=0;

	uint32_t codepoint;
	uint32_t state = 0;
	while (*src)
	{
		unsigned char ch=*src;
	    if (!decode(&state, &codepoint, ch))
	    {
   	    	++n;
	    }
		++src;
	}

    return (state != UTF8_ACCEPT)?-1:n;
}

