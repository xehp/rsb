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

#include <string>  // TODO can we forward declare std::string?
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

	void writeWord(const char* str); // Words are written unquoted (mostly used internally to write integers, longs and floats).
	void writeString(const char* str); // Strings are written as quoted strings.
	void writeStringW(const wchar_t* str); // Strings are written as quoted strings.
	void writeString(const std::string& str);
	void writeWord(const std::string& str);
	void writeInt(int i);
	void writeLong(long long i);
	void writeFloat(float f);

	// Some special for JSON
	// Examine the written file with: http://codebeautify.org/jsonviewer
	void writeArrayBegin() {addSeparatorIfNeeded();addChar('[');};
	void writeArrayEnd() {addChar(']');needSeparator=true;};
	void writeObjBegin() {addSeparatorIfNeeded(); addNewLine(); ++indentationLevel; addChar('{');};
	void writeObjEnd() {addChar('}');--indentationLevel; needSeparator=true;};
	void writeObjSeparator() {addChar(':');needSeparator=false;};
	void setSeparatorChar(char ch) {separatorChar=ch;}; // In JSON it shall be ',' but our default is a space char. So this needs to be called if JSON is to be used.

	const char *getString(); // This gives the string written (no more writing is allowed after the string is fetched)
	int getNBytes() const {return nBytes;};

	void addChar(int ch);
	void addSeparatorIfNeeded();
	void addNewLine();

	static int encodeString(char *dst, const char *src, int n);
	static int encodeStringW(char *dst, const wchar_t *src, int n);

protected:
	char *bufPtr;
	size_t bufSize;
	size_t nBytes;
	bool compleated;
	bool needSeparator;
	char separatorChar;
	int indentationLevel;
};


#endif
