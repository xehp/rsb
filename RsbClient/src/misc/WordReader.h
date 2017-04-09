/*
WordReader.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/
#ifndef WordReader_H
#define WordReader_H


#include <cstddef> // include needed for NULL. Or should we use nullptr instead of NULL. Read more at: http://stackoverflow.com/questions/462165/error-null-was-not-declared-in-this-scope
//#include <inttypes.h>


extern int wordReaderDebugCounter;
extern int wordReaderLinkDebugCounter;

class WordReaderLink
{
public:
	WordReaderLink(const unsigned int n, WordReaderLink *first);
	virtual ~WordReaderLink();
	char *bufPtr;
	WordReaderLink* next;
	int bufSizeDebug;
	static void cleanup();
	void debugAssert();
};


class WordReader
{
	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	WordReader(const WordReader&);
	WordReader& operator=(const WordReader&);


	enum TokenType
	{
		UnknowToken,
		NumberToken,
		NameToken,
		SimpleString,
		StringWithEscSeq,
		Opcode,
		atEnd
	};

	enum ReadTokenState
	{
		InitialState,
		ParsingNumber,
		ParsingName,
		NumberOrOpcode,
		ParsingOpcode,
		InsideStringNoEscSeq,
		InsideStringWithEscSeq,
		EscapeCharSeq,
		AfterEscapeCharSeq,
		EndState
	};


public:
	WordReader(const char *str);
	~WordReader();

	void skipSpaces();
	void skipNSpacesSeparator(const int n);
	// NOTE! Those methods that return a pointer gives a pointer to buffers inside the WordReader. The data there is valid only as long as the WordReader object exist. So if the data shall be kept after WordReader is deleted it must be copied.
	const char* lookAtString() const; // Does not move internal pointers
	const char* readWord();
	const char* readString();
	const char* readLine();
	const wchar_t* readLineW();
	int examineToken(const char **tokenPtr, TokenType *tokenType=NULL) const;
	int readToken(const char **tokenPtr, TokenType *tokenType=NULL);
	const char *readToken();
	const wchar_t *readTokenW();
	int readWord(char* strPtr, int strSize);
	#ifdef JSON_READER
	bool readExpected(const char *str);
	bool readObjBegin() {return readExpected("{");};
	bool readObjEnd() {return readExpected("}");};
	bool readObjSeparator() {return readExpected(":");};
	bool readArrayBegin() {return readExpected("[");};
	bool readArrayEnd() {return readExpected("]");};
	#endif
	int readInt();
	long long readLong();
	float readFloat();
	//double readDouble();
	bool readBoolean();
	bool equalsIgnoreCase(const char *str) const;
	bool endOfStr() const;
	bool isNext(const char *str) const;

	static void cpyW(wchar_t *dst, const char* src, size_t n);
	static int decodeString(char *dst, const char *src, int n);
	static int decodeStringW(wchar_t *dst, const char *src, int n);
	static int decodeUtf8(wchar_t *dst, const char *src, int n);
	static int countUtf8(const char *src);

private:
	//void addChar(int ch);

	const char *bufPtr;
	size_t bufSize;
	unsigned int readPos;
	WordReaderLink *first;
public:
	int separatorChar;

	static void cleanup();
};


#endif
