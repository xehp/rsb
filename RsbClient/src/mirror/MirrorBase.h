/*
MirrorBase.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef MirrorBase_H
#define MirrorBase_H

#include <cstddef> // include needed for NULL. Or should we use nullptr instead of NULL. Read more at: http://stackoverflow.com/questions/462165/error-null-was-not-declared-in-this-scope
#include <wchar.h>

// include needed for glm::mat4
/*
#ifndef __EMSCRIPTEN__
#include <GL/glew.h>
#include <glm/glm.hpp>
#else
#include <glew.h>
#include <glm.hpp>
#endif
*/

#define EMPIRE_CLIENT


extern int mirrorBaseDebugCounter;
extern int debugStringCopyFactory;

// TODO there is a better way: http://www.cplusplus.com/faq/sequences/arrays/sizeof-array/#cpp
#define SIZEOF_ARRAY( a ) (sizeof( a ) / sizeof( a[ 0 ] ))


class WordReader;
class MirrorDb;
class MirrorIdList;
//class D3PosAndOrientation;
#ifndef __EMSCRIPTEN__
class WordWriter;
#endif

class WindowObject;

char* stringCopyFactory(const char *str);
void stringCopyFree(char *str);
wchar_t* stringCopyFactoryW(const wchar_t *str);
void stringCopyFreeW(wchar_t *str);
void stringCopyCleanup();
void cpyW(wchar_t *dst, const char* src, size_t n);

class MirrorBase
{
public:
	MirrorBase();
	MirrorBase(int id, int index, int parentId);
	virtual ~MirrorBase();

	int getId() const {return id;};
	int getIndex() const {return index;};
	int getParentId() const {return parentId;};
	MirrorBase *getParentObj() const;
	const char* getName() const {return name;};
	void setName(const char*);
	virtual void renderAll() {/*renderSelf();*/};
	virtual void renderSelf(/*glm::mat4 &MVP*/) {};
	virtual void clear() {};
	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	virtual void writeJson(WordWriter *ww) = 0;
	#ifdef JSON_READER
	virtual int readJson(WordReader *wr, MirrorBase *parent) = 0;
	#endif
	#endif
	virtual void linkSelf(MirrorBase *parentPtr);
	virtual void addChildObject(MirrorBase *childObject) = 0;
	virtual void unlinkSelf();
	virtual void relinkSelf(MirrorBase *to) = 0;
	virtual void relinkSelf(int newParentId) = 0;
	virtual void removeChildObject(MirrorBase* childObject) = 0;
	virtual int getNChildObjects() const = 0;
	virtual MirrorBase * getChildByIndex(int childIndex) const = 0;
	//virtual D3PosAndOrientation* getPao() {return NULL;};
	virtual void setupInternalRenderingData() {};
	virtual MirrorBase * findChildByName(const char * childName, int recursionDepth) const = 0;
	//virtual bool isRecursiveParentOf(MirrorBase *o) = 0;
	virtual void dataChanged() {};
	//virtual void regNamedObject(MirrorBase *addedObject) {};
	virtual const char* getTypeName() const {return "MirrorBase";};
	//virtual void debugDump(int depth) const;
	virtual void regNamedChildObject(MirrorBase* childObj) {/*do nothing (unless overridden)*/;};
	//virtual long long getGameTime() const {mirrorDb->getWorld()->getGameTime();};
	virtual WindowObject* getWindowObject() {return parentPtr!=NULL?parentPtr->getWindowObject():NULL;}


	virtual void scrollUp() {};
	virtual void scrollDown() {};
	virtual void clearText() {};
	virtual const char* isButton(float f) {return NULL;};

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	MirrorBase(const MirrorBase&);
	MirrorBase& operator=(const MirrorBase&);

public:
	int id;
	int index; // TODO rename this to containingIndex or childIndex
	int parentId;
	int changedCounter;
private:
	char *name;
public:
	MirrorBase *parentPtr;
	MirrorIdList *idListPtr;
};

#endif
