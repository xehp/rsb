/*
MirrorContainer.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "WordReader.h"
#include "MirrorDb.h"
#include "MirrorContainer.h"


MirrorContainer::MirrorContainer(int id, int index, int parentId):
	MirrorBase(id, index, parentId),
	childVector(0)
{
};


MirrorContainer::~MirrorContainer()
{
	//printf("~MirrorContainer: %s\n", this->getName());

	if (childVector.size()!=0)
	{
		unlinkAllChildObjects();
	}

	assert(getNChildObjects()==0);
}


// All child objects are unlinked from all lists and also deleted from memory.
// TODO rename this to deleteAllChildObjects()
void MirrorContainer::unlinkAllChildObjects()
{
	MIRROR_DEBUG_ASSERT();

	//printf("delete all children for %s ~%d %p\n", name, id, mirrorDb);
	//assert( (mirrorDb==NULL) || (this==mirrorDb->getObjectById(id)) || (id<0) );
	for(size_t i=0; i<childVector.size();++i)
	{
		MirrorBase *mb=childVector[i];
		if (mb!=NULL)
		{
			// make sure database is still consistent
			assert(mb->getParentId()==this->id);
			//assert((mb->getParentObj()==this) || (mb->getParentObj()==NULL));
			assert((mb->mirrorDb==this->mirrorDb) || (this->mirrorDb==NULL));
			assert(mb->getIndex()==i);

			//mb->unlinkSelf(); // Don't use unlinkSelf, the destructor will do all that is needed.
			delete(mb);
			childVector[i]=NULL;
		}
	}
}


// When testing on web I get null pointer exception in this method. But why? From the logs objects are being removed due to messages from server. But I thought this is single threaded. Should perhaps try queuing up messages and handle them in main loop only.
void MirrorContainer::renderAll()
{
	renderSelf();

	for(size_t i=0; i<childVector.size();++i)
	{
		if (childVector[i]!=NULL)
		{
			(childVector[i])->renderAll();
		}
	}
}


// Add an object as a child to this object.
void MirrorContainer::addChildObject(MirrorBase *childObject)
{
	int index = childObject->getIndex();

	if (index<0)
	{
		// The child object does not have an index, find a free slot (AKA index) for it.
		index=findFreeSlot();
		childObject->index=index;
	}


	assert(index>=0); // For debugging, can be removed later

	// The list (AKA vector) must be large enough
	while (index>=childVector.capacity())
	{
		// Need a bigger list AKA vector

		const size_t s = childVector.capacity();
		if (s==0)
		{
			childVector.resize(1);
		}
		else
		{
			childVector.resize(s*2);
		}

		//childList = new MirrorBase*[childListSize]; // http://stackoverflow.com/questions/2265664/how-to-dynamically-allocate-the-array-of-pointers-in-c

		// initialize all new slots to NULL
		for(size_t i=s; i<childVector.capacity();++i)
		{
			childVector[i]=NULL;
		}
	}

	if (childVector[index]!=NULL)
	{
		childVector[index]=NULL;
	}

	// Now the child object has an index and the list is big enough, so store the pointer to the child object
	childVector[index]=childObject;
	childObject->index=index;
	childObject->parentId=id;


	// Database consistency check, this was added for debugging and can be removed later
	if ((index<0) || (index>=childVector.capacity()) || (childObject->parentId!=this->id) || (childObject!=childVector[index]))
	{
		printf("data inconsistent ~%d ~%d ~%d %d %d %d\n", this->id, childObject->parentId, childObject->id, index, (int)childVector.size(), (int)childVector.capacity());
	}


	// If the object added was one with a reserved name it shall perhaps be registered in some way?
	const char *tmp=childObject->getName();
	if ((tmp!=NULL) && (tmp[0]=='_'))
	{
		// This is a fixed sub object with a reserved name.

		regNamedObject(childObject);
	}

}

void MirrorContainer::readSelf(WordReader *wr)
{
	MirrorBase::readSelf(wr);

	// The Lua script info is not needed by client, just read and forget it.
	wr->readString();
}


// A child object is removed from the list (it is not deleted from memory or removed from other list such as the ID list)
void MirrorContainer::removeChildObject(MirrorBase* childObject)
{
	MIRROR_DEBUG_ASSERT();

	const int index = childObject->getIndex();

	// Database consistency check, this is just to help debugging.
	if ((index<0) || (index>=childVector.capacity()))
	{
		printf("index out of range ~%d ~%d ~%d %d %d %d\n",this->id, childObject->parentId,childObject->id, index, (int)childVector.size(), (int)childVector.capacity());
		return;
	}
	else if ((childObject->parentId!=this->id) || (childObject!=childVector[index]))
	{
		printf("inconsistent data ~%d ~%d ~%d %d %d %d\n",this->id, childObject->parentId,childObject->id, index, (int)childVector.size(), (int)childVector.capacity());
	}

	// These lines where added during debugging, is probably not needed and can be removed when it is time to optimize code.
	assert(index>=0);
	assert(index<childVector.size());
	assert(childObject==childVector[index]);
	assert(childObject->parentId==this->id);

	childVector[index]=NULL;


	// These lines where added during debugging, should not be needed and can perhaps be removed when it is time to optimize code. For now MirrorBase will assert it was done here.
	childObject->index=-1;
	childObject->parentId=-1;
}

// counts number of child objects
int MirrorContainer::getNChildObjects() const
{
	MIRROR_DEBUG_ASSERT();

	//const size_t s=childVector.size();

	size_t n=0;
	size_t c=childVector.capacity();
	for(size_t i=0; i<c;++i)
	{
		if (childVector[i]!=NULL)
		{
			++n;
		}
	}
	/*if(s!=n)
	{
		// TODO The way we use std::vector it seems size is not what is expected. It is instead same as capacity. Perhaps something more efficient can be done here.
		printf("s!=n: %d %d\n", (int)s, (int)n);
	}*/
	//assert(s==n);

	return n;
}

// Don't forget to do delete also if object is no longer to be used.
void MirrorContainer::unlinkSelf()
{
	MIRROR_DEBUG_ASSERT();

	unlinkAllChildObjects(); // Some extending classes may want to move child object to parent object instead.
	MirrorBase::unlinkSelf();
}

// protected internal help function
int MirrorContainer::findFreeSlot()
{
	int i=0;
	while(i<childVector.size())
	{
		if (childVector[i]==NULL)
		{
			return i;
		}
		++i;
	}
	return i;
}


MirrorBase * MirrorContainer::getChildByIndex(int childIndex) const
{
	assert(childIndex>=0);
	if (childIndex<childVector.size())
	{
		return childVector[childIndex];
	}
	else
	{
		return NULL;
	}
}


// This can be slow, use getChildByIndex if possible. This will search recursively if recursionDepth is larger than zero.
MirrorBase * MirrorContainer::findChildByName(const char * childName, int recursionDepth) const
{
	assert(childName!=NULL);
	for(int i=0; i<childVector.size();++i)
	{
		MirrorBase *mb=childVector[i];
		if (mb!=NULL)
		{
			const char *tmp=mb->getName();
			if (strcmp(tmp, childName)==0)
			{
				// object found
				return mb;
			}
			else if (recursionDepth>0)
			{
				MirrorBase *c = mb->findChildByName(childName, recursionDepth-1);
				if (c!=NULL)
				{
					return c;
				}
			}

		}
	}
	return NULL;
}

/*
bool MirrorContainer::isRecursiveParentOf(MirrorBase *o)
{
	MirrorBase *dc=o->getParentObj();
	if (dc==NULL)
	{
		return false;
	}
	if (dc==this)
	{
		return true;
	}
	return isRecursiveParentOf(dc);
}*/


// Move this object from its current container to another container.
// Returns zero if OK
void MirrorContainer::moveToNewParent(int newParentId)
{

	if (newParentId == this->id)
	{
		printf("Can not move into itself.\n");
		return;
	}

	MirrorBase *to=mirrorDb->getObjFromIndex(newParentId);

	// Make sure an object is not put into something inside itself.
	/*if (this->isRecursiveParentOf(to))
	{
		printf("Can not move into a child (or child of a child).\n");
		return;
	}*/

	const int currentParentId = this->parentId;

	MirrorBase *from=mirrorDb->getObjFromIndex(currentParentId);
	from->removeChildObject(this);
	to->addChildObject(this);
	return;
}



void MirrorContainer::debugDump(int depth) const
{
	MIRROR_DEBUG_ASSERT();

	MirrorBase::debugDump(depth);
	printf("%d {\n", depth);
	for(int i=0; i<childVector.size();++i)
	{
		MirrorBase *mb=childVector[i];
		if (mb!=NULL)
		{
			assert(mb->mirrorDb!=NULL);
			assert((mb->mirrorDb==this->mirrorDb) || (this->mirrorDb==NULL));
			assert(mb->getParentId()==this->getId());
			assert(i==mb->getIndex());
			assert((mb->getParentObj()==this) || (mb->getParentObj()==NULL));

			mb->debugDump(depth+1);
		}
	}
	printf("%d }\n",depth);
}


