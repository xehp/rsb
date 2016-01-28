/*
WordWriter.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/
#ifndef WordWriter_H
#define WordWriter_H


#include <cstddef> // include needed for NULL. Or should we use nullptr instead of NULL. Read more at: http://stackoverflow.com/questions/462165/error-null-was-not-declared-in-this-scope

// TODO: Rename this class to WordWriter

class WordWriter
{
	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	WordWriter(const WordWriter&);
	WordWriter& operator=(const WordWriter&);

public:
	WordWriter(size_t bufSize=64);
	~WordWriter();

	// These gives a pointer to next string in the buffer. The data there is valid only as long as the WordWriter object exist. So if the data shall be saved it must be copied.
	void writeWord(const char* str);
	void writeString(const char* str);

	void writeInt(int i);
	void writeLong(long long i);
	void writeFloat(float f);

	const char *getString();

private:
	void addChar(int ch);

	char *bufPtr;
	size_t bufSize;
	size_t nBytes;
	bool compleated;

};


#endif
