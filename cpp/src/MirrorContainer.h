/*
MirrorContainer.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
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
public:
	MirrorContainer() : childVector() {};
	MirrorContainer(int id, int index, int parentId);

	virtual ~MirrorContainer();

	virtual void renderAll();
	virtual void renderSelf(/*glm::mat4 &MVP*/) {};
	virtual void unlinkAllChildObjects();
	virtual void readSelf(WordReader *wr);
	virtual void addChildObject(MirrorBase *childObject);
	virtual void removeChildObject(MirrorBase* childObject);
	virtual void unlinkSelf();
	virtual MirrorBase * getChildByIndex(int childIndex) const;
	virtual MirrorBase * getObjFromIndex(int objIndex) const {return getChildByIndex(objIndex);}
	virtual MirrorBase * findChildByName(const char * childName, int recursionDepth) const;
	virtual int getNChildObjects() const;
	//virtual bool isRecursiveParentOf(MirrorBase *o);
	virtual void moveToNewParent(int newParentId);
	virtual const char* getTypeName() const {return "MirrorContainer";};
	virtual void debugDump(int depth) const;

	std::vector<MirrorBase *> childVector;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	MirrorContainer(const MirrorContainer&);
	MirrorContainer& operator=(const MirrorContainer&);

	int findFreeSlot();

};

#endif
