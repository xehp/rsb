/*
WordString.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef WordString_H
#define WordString_H

#include <cstddef> // include needed for NULL. Or should we use nullptr instead of NULL. Read more at: http://stackoverflow.com/questions/462165/error-null-was-not-declared-in-this-scope

// TODO: Rename this class to WordReader

class WordString
{
	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	WordString(const WordString&);
	WordString& operator=(const WordString&);

public:
	WordString(const char *str);
	~WordString();

	// Gives a pointer to next string in the buffer. The data there is valid only until the WordString object exist. So if the data shall be saved it must be copied.
	const char* lookAtString(); // Does not move internal pointers
	const char* getNextString(); // Moves internal pointer to next string.
	int readInt();
	long long readLong();
	const char* readWord() {return getNextString();}
	const char* readString() {return getNextString();}
	float readFloat();
	//double readDouble();

	bool equalsIgnoreCase(const char *str);

private:
	void addChar(int ch);

	char *bufPtr;
	size_t bufSize;
	size_t nBytes;
	unsigned int nWords;
	unsigned int readPos;


};


#endif
