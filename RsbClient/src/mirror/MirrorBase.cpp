/*
MirrorBase.cpp


www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "HybridLib.h"
#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "MirrorIdList.h"
#include "MirrorBase.h"

int mirrorBaseDebugCounter=0;
int debugStringCopyFactory=0;

// Copy byte string to wide string
void cpyW(wchar_t *dst, const char* src, size_t n)
{
	while(n>0)
	{
		*dst=*src;
		++dst;
		++src;
		--n;
	}
}


// This allocates memory and copies a string to it.
// Remember to do free (by calling stringCopyFree) on the memory when no longer needed.
char* stringCopyFactory(const char *str)
{
	if(str!=NULL)
	{
		const int len=strlen(str)+1; // +1 for trailing zero
		char* ptr = (char*)malloc(len); // caller is responsible to free this, that is why this is called "factory"
		assert(ptr!=NULL);
		debugStringCopyFactory++;
		memcpy(ptr, str, len);
		return ptr;
	}
	else
	{
		return NULL;
	}
}

void stringCopyFree(char *str)
{
	if (str!=NULL)
	{
		free(str);
		debugStringCopyFactory--;
	}
}


wchar_t* stringCopyFactoryW(const wchar_t *str)
{
	if(str!=NULL)
	{
		const int len=wcslen(str)+1; // +1 for trailing zero
		wchar_t* ptr = (wchar_t*)malloc(len*sizeof(wchar_t)); // caller is responsible to free this, that is why this is called "factory"
		assert(ptr!=NULL);
		debugStringCopyFactory++;
		wmemcpy(ptr, str, len);
		return ptr;
	}
	else
	{
		return NULL;
	}
}

void stringCopyFreeW(wchar_t *str)
{
	if (str!=NULL)
	{
		free(str);
		debugStringCopyFactory--;
	}
}


void stringCopyCleanup()
{
	assert(debugStringCopyFactory==0);
}


MirrorBase::MirrorBase() :
#ifdef MIRROR_DEBUG
	debug0(DEBUG0),
#endif
	id(-1),
	index(-1),
	parentId(-1),
	changedCounter(0),
	name(NULL),
	parentPtr(NULL),
	idListPtr(NULL)
#ifdef MIRROR_DEBUG
	,debug1(DEBUG1)
#endif
{
	++mirrorBaseDebugCounter;
};

MirrorBase::MirrorBase(int id, int index, int parentId) :
	id(id),
	index(index),
	parentId(parentId),
	changedCounter(0),
	name(NULL),
	parentPtr(NULL),
	idListPtr(NULL)
{
	++mirrorBaseDebugCounter;
};



MirrorBase::~MirrorBase()
{
	//printf("~MirrorBase: %s\n", this->getName());

	if (parentPtr!=NULL)
	{
		unlinkSelf();
	}

	stringCopyFree(name);

	// The following is for debugging, can be removed later to make code faster.
	parentPtr=NULL;
	name=NULL;
	id=-1;
	index=-1;
	parentId=-1;
	changedCounter=-1;
	parentPtr=NULL;
#ifdef MIRROR_DEBUG
	debug0=0;
	debug1=0;
#endif
	--mirrorBaseDebugCounter;
	assert(mirrorBaseDebugCounter>=0);
}



void MirrorBase::readSelf(WordReader *wr)
{
	// See: containingIndex in DbStorable.java
	index = wr->readInt();

	// If we are to use JSON this name must be quoted, so use readString instead of readWord
	setName(wr->readString());

	// See DbIdObj.java
	// Server sends ID twice. That is superfluous. But is it a server issue. Check at least that it is same id.
	const int tmpId = id;
	id = wr->readInt();
	/*if ((tmpId!=-1) && (tmpId != id))
	{
		printf("MirrorBase::readSelf: id: %d != %d\n", tmpId, id);
		fflush(stdout);
	}*/
	assert((tmpId==-1) || (tmpId == id));
}

#ifndef __EMSCRIPTEN__
void MirrorBase::writeSelf(WordWriter *ww)
{
	// See: containingIndex in DbStorable.java
	ww->writeInt(index);

	// If we are to use JSON this name must be quoted, so use readString instead of readWord
	ww->writeString(getName());

	// See DbIdObj.java
	// Server expect ID twice. That is superfluous. But is it a server issue. Check at least that it is same id.
	ww->writeInt(id);

}
#endif


void MirrorBase::linkSelf(MirrorBase *parentPtr)
{
	assert(parentPtr!=NULL);
	assert(this->parentPtr==NULL);

	// This is last. No super class linkSelf method to call from here.

	// Register with parent
	parentPtr->addChildObject(this);

	assert(index>=0);
	assert(this->parentPtr == parentPtr);
	assert(parentPtr->getChildByIndex(index)==this);

	this->idListPtr = parentPtr->idListPtr;


	if (this->idListPtr!=NULL)
	{
		this->idListPtr->registerIdObject(this);

		// TODO: If this object have child objects should we register their ID in mirrorDb. For now will assert that there are no child objects.
		assert(getNChildObjects()==0);

		assert(idListPtr->getObjectById(id)==this);
	}

	// If the object has a reserved name perhaps some special handling is needed.
	// TODO we seem to do this in void MirrorContainer::addChildObject(MirrorBase *childObject) also.
	if ((this->name!=NULL) && (this->name[0]=='_'))
	{
		parentPtr->regNamedChildObject(this);
	}


	assert(parentPtr->getChildByIndex(this->index)==this);
}

// A child object is removed from all list where it is registered, parents list and ID list. It is not deleted from memory, so delete is needed also.
// Extending classes needs to override this and call this before or after unlinking its lists.
void MirrorBase::unlinkSelf()
{
	assert(parentPtr!=NULL); // If this assert fails then perhaps the object was not linked when it should have been?

	// This is last. No super class unlinkSelf method to call from here.
	//printf("unlinkSelf: name=%s, id=~%d, parent=~%d, db=%s\n", name, id, parentId, mirrorDb->getName());

	// unlink from IdList
	if (idListPtr!=NULL)
	{
		assert(id>=0);
		idListPtr->unregisterIdObject(this);

		assert(id<0);
		assert(idListPtr==NULL);
	}
	else
	{
		assert(id<0);
	}



	assert((parentId<0) || (parentPtr->id == parentId));


	// unlink from parent
	parentPtr->removeChildObject(this);
	assert(parentPtr==NULL);
	assert(parentId==-1);
	assert(index<0);

}

MirrorBase *MirrorBase::getParentObj() const
{
	assert((idListPtr==NULL) || (parentPtr==idListPtr->getObjectById(parentId)));
	return parentPtr;
}

void MirrorBase::setName(const char* newName)
{

	if (name!=NULL)
	{
		if (strcmp(newName, name)!=0)
		{
			stringCopyFree(name);
			name = stringCopyFactory(newName); // stringCopyFree is done in destructor
		}
		else
		{
			// name is same, do nothing
		}
	}
	else
	{
		name = stringCopyFactory(newName); // stringCopyFree is done in destructor
	}

}

/*
void MirrorBase::debugDump(int depth) const
{
	MIRROR_DEBUG_ASSERT();
	if (mirrorDb!=NULL)
	{
		assert(mirrorDb->getObjectById(id)==this);

		if (parentId>=0)
		{
			assert(index>=0);
			MirrorBase *p=mirrorDb->getObjectById(parentId);
			assert(p!=NULL);
		}
		else
		{
			assert(parentId==-1);
		}


	}
	else
	{
		assert(parentId==-1);

	}


	printf("%d %s ~%d\n", depth, getName(), getId());
}
*/



