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

extern int debugCounter;

// TODO there is a better way: http://www.cplusplus.com/faq/sequences/arrays/sizeof-array/#cpp
#define SIZEOF_ARRAY( a ) (sizeof( a ) / sizeof( a[ 0 ] ))

#ifdef MIRROR_DEBUG
#define DEBUG0 18748654 // just some randomly selected magic numbers
#define DEBUG1 44884654
#define MIRROR_DEBUG_ASSERT() assert(debug0==DEBUG0 && debug1==DEBUG1)
#else
#define MIRROR_DEBUG_ASSERT()
#endif

class WordReader;
class MirrorDb;
//class D3PosAndOrientation;

char* stringCopyFactory(const char *str);

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
	virtual void linkSelfIntoDb(MirrorDb *mirrorDb);
	virtual void addChildObject(MirrorBase *childObject) = 0;
	virtual void unlinkSelf();
	virtual void moveToNewParent(int newParentId) = 0;
	virtual void removeChildObject(MirrorBase* childObject) = 0;
	virtual int getNChildObjects() const = 0;
	//virtual D3PosAndOrientation* getPao() {return NULL;};
	virtual void setupInternalRenderingData() {};
	virtual MirrorBase * findChildByName(const char * childName, int recursionDepth) const = 0;
	//virtual bool isRecursiveParentOf(MirrorBase *o) = 0;
	virtual void dataChanged() {};
	virtual void regNamedObject(MirrorBase *addedObject) {};
	virtual const char* getTypeName() const {return "MirrorBase";};
	virtual void debugDump(int depth) const;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	MirrorBase(const MirrorBase&);
	MirrorBase& operator=(const MirrorBase&);

public:
#ifdef MIRROR_DEBUG
	int debug0;
#endif
	int id;
	int index; // TODO rename this to containingIndex or childIndex
	int parentId;
	int changedCounter;
private:
	char *name;
public:
	MirrorDb *mirrorDb;
#ifdef MIRROR_DEBUG
	int debug1;
#endif
};

#endif
