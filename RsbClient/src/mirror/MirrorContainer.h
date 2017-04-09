/*
MirrorContainer.h

Mirror Container is a mirror object that can contain other mirror objects.


www.eit.se/rsb
Copyright (C) 2017 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef MirrorContainer_H
#define MirrorContainer_H

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

#include <vector>
#include "MirrorBase.h"

class MirrorDb;

class MirrorContainer: public MirrorBase
{
	enum{
		debugContainerMagicNumber=985652
	};

public:
	MirrorContainer();
	//MirrorContainer(int id, int index, int parentId);

	virtual ~MirrorContainer();

	virtual void renderAll();
	virtual void renderSelf(/*glm::mat4 &MVP*/) {};
	virtual void unlinkAllChildObjects();
	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	virtual void writeJson(WordWriter *ww);
	#ifdef JSON_READER
	virtual int readJson(WordReader *wr, MirrorBase *parent);
	#endif
	#endif
	virtual void addChildObject(MirrorBase *childObject);
	virtual void removeChildObject(MirrorBase* childObject);
	virtual void unlinkSelf();
	virtual MirrorBase * getChildByIndex(int childIndex) const;
	virtual MirrorBase * getObjFromIndex(int objIndex) const {return getChildByIndex(objIndex);}
	virtual MirrorBase * findChildByName(const char * childName, int recursionDepth) const;
	virtual int getNChildObjects() const;
	//virtual bool isRecursiveParentOf(MirrorBase *o);
	virtual void relinkSelf(MirrorBase *to);
	virtual void relinkSelf(int newParentId);
	virtual const char* getTypeName() const {return "MirrorContainer";};
	//virtual void debugDump(int depth) const;

	std::vector<MirrorBase *> childVector; // TODO childVector should be protected

	// Prevent unintentional use of copy-constructor and assignment operator by making those private.
protected:
	MirrorContainer(const MirrorContainer&);
	MirrorContainer& operator=(const MirrorContainer&);

	int findFreeSlot();

	int debugContainer;
	int nChildObjects;
};

#endif
