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

// TODO: Rename this class to WordReader

class WordReader
{
	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	WordReader(const WordReader&);
	WordReader& operator=(const WordReader&);

public:
	WordReader(const char *str);
	~WordReader();

	// These gives a pointer to next string in the buffer. The data there is valid only as long as the WordReader object exist. So if the data shall be saved it must be copied.
	const char* lookAtString() const; // Does not move internal pointers
	const char* readWord();
	const char* readString();
	const char* readLine();

	int readInt();
	long long readLong();
	float readFloat();
	//double readDouble();
	bool readBoolean();

	bool equalsIgnoreCase(const char *str);
	bool endOfStr();

private:
	//void addChar(int ch);

	char *bufPtr;
	size_t bufSize;
	size_t nBytes;
	unsigned int nWords;
	unsigned int readPos;


};


#endif
