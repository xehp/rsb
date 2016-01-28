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
#include "MirrorDb.h"
#include "MirrorBase.h"

int debugCounter=0;

MirrorBase::MirrorBase() :
#ifdef MIRROR_DEBUG
	debug0(DEBUG0),
#endif
	id(-1),
	index(-1),
	parentId(-1),
	changedCounter(0),
	name(NULL),
	mirrorDb(NULL)
#ifdef MIRROR_DEBUG
	,debug1(DEBUG1)
#endif
{
	++debugCounter;
};

MirrorBase::MirrorBase(int id, int index, int parentId) :
#ifdef MIRROR_DEBUG
	debug0(DEBUG0),
#endif
	id(id),
	index(index),
	parentId(parentId),
	changedCounter(0),
	name(NULL),
	mirrorDb(NULL)
#ifdef MIRROR_DEBUG
	,debug1(DEBUG1)
#endif
{
	++debugCounter;
};



MirrorBase::~MirrorBase()
{
	MIRROR_DEBUG_ASSERT();

	//printf("~MirrorBase: %s\n", this->getName());

	if (mirrorDb!=NULL)
	{
		unlinkSelf();
	}

	free(name);

	// The following is for debugging, can be removed later to make code faster.
	name=NULL;
	id=-1;
	index=-1;
	parentId=-1;
	changedCounter=-1;
	mirrorDb=NULL;
#ifdef MIRROR_DEBUG
	debug0=0;
	debug1=0;
#endif
	--debugCounter;
	assert(debugCounter>=0);
}

// This allocates memory and copies a string to it.
// Remember to do free on the memory when no longer needed.
char* stringCopyFactory(const char *str)
{
	if(str!=NULL)
	{
		const int len=strlen(str)+1; // +1 for trailing zero
		char* ptr = (char*)malloc(len); // caller is responsible to free this, that is why this is called "factory"
		assert(ptr!=NULL);
		memcpy(ptr, str, len);
		return ptr;
	}
	else
	{
		return NULL;
	}
}


void MirrorBase::readSelf(WordReader *wr)
{
	// See: containingIndex in DbStorable.java
	index = wr->readInt();

	// If we are to use JSON this name must be quoted, so use readString instead of readWord
	setName(wr->readString());

	// See DbIdObj.java
	// Server sends ID twice. That is superfluous. But is it a server issue. Check at least that it is same id.
	const int tmpId = wr->readInt();
	if (tmpId != id)
	{
		printf("MirrorBase::readSelf: id: %d != %d\n", tmpId, id);
		id = tmpId;
	}
}


// Parameter mirrorDb tells which database we are working with. That is needed since parent object IDs are valid within a database only.
// It will find its parent if any within the given database.
// NOTE Unlike in the server the parameter here shall not be the parent object but the database.
void MirrorBase::linkSelfIntoDb(MirrorDb *mirrorDb)
{
	MIRROR_DEBUG_ASSERT();
	assert(mirrorDb!=NULL);

	// This is last. No super class linkSelf method to call from here.

	this->mirrorDb = mirrorDb;

	mirrorDb->registerIdObject(this);

	// TODO: If this object have child objects should we register their ID in mirrorDb. For now will assert that there are no child objects.
	assert(getNChildObjects()==0);

	if (parentId>=0)
	{
		MirrorBase *parentPtr = mirrorDb->getObjectById(parentId);
		if (parentPtr!=NULL)
		{
			parentPtr->addChildObject(this);
		}
		else
		{
			printf("parent not found %d\n",parentId);
		}
		assert(parentPtr->mirrorDb==mirrorDb);
	}
	else
	{
		// If it has no parent it shall go to database root.

		// TODO: The index we get from server in this case is not useful here since we don't have the object above this.
		//index = 0;

		mirrorDb->addChildObject(this);

		//printf("MirrorContainer::linkSelf: error: unknown parent %d\n", parentId);
		assert(mirrorDb->getObjFromIndex(index)==this);
	}



	assert(mirrorDb->getObjectById(id)==this);
}

// A child object is removed from all list where it is registered, parents list and ID list. It is not deleted from memory, so delete is needed also.
// Extending classes needs to override this and call this before or after unlinking its lists.
void MirrorBase::unlinkSelf()
{
	MIRROR_DEBUG_ASSERT();
	assert(mirrorDb!=NULL);

	// This is last. No super class unlinkSelf method to call from here.
	//printf("unlinkSelf: name=%s, id=~%d, parent=~%d, db=%s\n", name, id, parentId, mirrorDb->getName());

	// unlink from parent
	if (parentId>=0)
	{
		MirrorBase *parentPtr = mirrorDb->getObjectById(parentId);

		assert(parentPtr->id == parentId);
		MIRROR_DEBUG_ASSERT();

		if (parentPtr==NULL)
		{
			printf("did not find parent ~%d\n",parentId);
		}
		else
		{
			//printf("remove this from: name=%s, id=~%d\n", parentPtr->getName(), parentPtr->id);
			parentPtr->removeChildObject(this);
		}

		// Perhaps these two should be reset here but for now we assert that MirrorContainer will do it.
		assert(parentId==-1);
		assert(index<0);
		//parentId=-1;
		//index=-1;
	}
	else
	{
		// If it has no parent then its in database root.
		mirrorDb->removeChildObject(this);

		// Perhaps this should be reset here but for now we assert that MirrorDb will do it (because assert is good).
		assert(parentId==-1);
		assert(index<0);
		//index=-1;
	}


	// unlink from database
	if (id>=0)
	{
		mirrorDb->unregisterIdObject(this);

		// Currently MirrorDb will set these two. Perhaps later it will be done here because these two attributes belong to here.
		assert(id<0);
		assert(mirrorDb==NULL);
		//id=-1;
		//mirrorDb=NULL;
	}
	else
	{
		assert(mirrorDb==NULL);
	}

}

MirrorBase *MirrorBase::getParentObj() const
{
	assert(mirrorDb!=NULL);
	return (mirrorDb!=NULL)?mirrorDb->getObjectById(parentId):NULL;
}

void MirrorBase::setName(const char* newName)
{
	MIRROR_DEBUG_ASSERT();

	if (name!=NULL)
	{
		if (strcmp(newName, name)!=0)
		{
			free(name);
			name = stringCopyFactory(newName); // free is done in destructor: "free(name)".
		}
		else
		{
			// name is same, do nothing
		}
	}
	else
	{
		name = stringCopyFactory(newName); // free is done in destructor: "free(name)".
	}

	MIRROR_DEBUG_ASSERT();
}


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



