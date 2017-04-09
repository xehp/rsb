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
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#ifdef JSON_READER
#include "MirrorFactory.h"
#endif
#endif
#include "MirrorIdList.h"
#include "MirrorContainer.h"


MirrorContainer::MirrorContainer():
childVector(4),
debugContainer(debugContainerMagicNumber),
nChildObjects(0)
{
}

/*
MirrorContainer::MirrorContainer(int id, int index, int parentId):
	MirrorBase(id, index, parentId),
	MirrorContainer()
{
};
*/

MirrorContainer::~MirrorContainer()
{
	assert(debugContainer==debugContainerMagicNumber);
	//printf("~MirrorContainer: %s\n", this->getName());

	if (getNChildObjects()!=0)
	{
		unlinkAllChildObjects();
	}

	assert(getNChildObjects()==0);
}


// All child objects are unlinked from all lists and also deleted from memory.
// TODO rename this to deleteAllChildObjects()
void MirrorContainer::unlinkAllChildObjects()
{
	//printf("delete all children for %s ~%d %p\n", name, id, mirrorDb);
	//assert( (mirrorDb==NULL) || (this==mirrorDb->getObjectById(id)) || (id<0) );
	for(size_t i=0; i<childVector.size();++i)
	{
		MirrorBase *mb=childVector[i];
		if (mb!=NULL)
		{
			// make sure database is still consistent
			assert(mb->getIndex()==i);

			mb->unlinkSelf(); // Or don't use unlinkSelf, the destructor will do all that is needed.
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
// This will not do all other linking stuff, for that use linkSelf (which will call this among other things).
void MirrorContainer::addChildObject(MirrorBase *childObject)
{
	int index = childObject->getIndex();

	if (index<0)
	{
		// The child object does not have an index, find a free slot (AKA index) for it.
		index=findFreeSlot();
		childObject->index=index;
	}

	assert(childObject->parentPtr==NULL);
	assert(index>=0); // For debugging, can be removed later

	// The list (AKA vector) must be large enough
	while (index>=childVector.size())
	{
		// Need a bigger list AKA vector


		const size_t svc = childVector.size();
		if (svc==0)
		{
			childVector.resize(1);
		}
		else
		{
			childVector.resize(svc*2);
		}

		//childList = new MirrorBase*[childListSize]; // http://stackoverflow.com/questions/2265664/how-to-dynamically-allocate-the-array-of-pointers-in-c

		// initialize all new slots to NULL
		for(size_t i=svc; i<childVector.size();++i)
		{
			childVector[i]=NULL;
		}
	}

	if (childVector[index]!=NULL)
	{
		printf("MirrorContainer::addChildObject: That index was not free. index=%d\n", index);
		fflush(stdout);
		childVector[index]=NULL;
		nChildObjects--;
	}

	// Now the child object has an index and the list is big enough, so store the pointer to the child object
	childVector[index]=childObject;
	childObject->index=index;
	childObject->parentPtr=this;
	childObject->parentId=this->id;
	++nChildObjects;


	// just for debugging
	const int cvs=childVector.size();
	assert(nChildObjects<=cvs);
	assert(debugContainer==debugContainerMagicNumber);
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
	const int index = childObject->getIndex();

	// These lines where added during debugging, is probably not needed and can be removed when it is time to optimize code.
	assert(index>=0);
	assert(index<childVector.size());
	assert(childObject==childVector[index]);

	childVector[index]=NULL;


	// These lines where added during debugging, should not be needed and can perhaps be removed when it is time to optimize code. For now MirrorBase will assert it was done here.
	childObject->index=-1;
	childObject->parentId=-1;
	childObject->parentPtr=NULL;

	--nChildObjects;

	// just for debugging
	const int s=childVector.size();
	assert(nChildObjects<=s);
	assert(debugContainer==debugContainerMagicNumber);
}

// counts number of child objects
int MirrorContainer::getNChildObjects() const
{

	// The following is for debugging, can be removed later.
	/*
	size_t n=0;
	const size_t c=childVector.size();
	for(size_t i=0; i<c;++i)
	{
		if (childVector[i]!=NULL)
		{
			++n;
		}
	}
	assert(nChildObjects==n);
	*/

	return nChildObjects;
}

// Don't forget to do delete also if object is no longer to be used.
void MirrorContainer::unlinkSelf()
{
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


void MirrorContainer::relinkSelf(MirrorBase *to)
{

	// TODO
	// Make sure an object is not put into something inside itself.
	/*if (this->isRecursiveParentOf(to))
	{
		printf("Can not move into a child (or child of a child).\n");
		return;
	}*/

	assert(parentPtr==idListPtr->getObjFromIndex(this->parentId));
	parentPtr->removeChildObject(this);
	to->addChildObject(this);
	return;
}


// Move this object from its current container to another container.
// Returns zero if OK
void MirrorContainer::relinkSelf(int newParentId)
{

	if (newParentId == this->id)
	{
		printf("Can not move into itself.\n");
		return;
	}

	assert(idListPtr!=NULL);

	MirrorBase *to=idListPtr->getObjectById(newParentId);

	relinkSelf(to);

	return;
}


/*
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
*/

#ifndef __EMSCRIPTEN__
void MirrorContainer::writeSelf(WordWriter *ww)
{
	MirrorBase::writeSelf(ww);

	// The Lua script info is not stored by client, write an empty string in its place for now.
	ww->writeString("");
}

void MirrorContainer::writeJson(WordWriter *ww)
{
	ww->writeObjSeparator();


	// Write and array with two elements (those are also arrays)
	ww->writeArrayBegin();


	ww->writeArrayBegin();
	// Write own data in first array
	writeSelf(ww);
	ww->writeArrayEnd();


	// Write children in second array
	if (getNChildObjects()>0)
	{
		ww->writeArrayBegin();
		for(int i=0; i<childVector.size();++i)
		{
			MirrorBase *mb=childVector[i];
			if (mb!=NULL)
			{
				ww->writeObjBegin();
				ww->writeString(mb->getTypeName());
				mb->writeJson(ww);
				ww->writeObjEnd();
			}
		}
		ww->writeArrayEnd();
	}

	ww->writeArrayEnd();

}

#ifdef JSON_READER
// Return a negative value if not OK
int MirrorContainer::readJson(WordReader *wr, MirrorBase *parent)
{
	// First we expect a ':' character.
	if (!wr->readObjSeparator()) return -1;

	// We expect now a main array with one or two arrays inside.
	if (!wr->readArrayBegin()) return -1;

	// This begins the first array (the one with objects own data)
	if (!wr->readArrayBegin()) return -1;

	// It contains all data about this object-
	readSelf(wr);

	this->linkSelf(parent);

	// This ends the first array (the one with objects own data)
	if (!wr->readArrayEnd()) return -1;

	// Now it can be "[" if there are child objects or "]" if no child objects.

	if (wr->isNext("["))
	{
		// This begins the child objects array
		if (!wr->readArrayBegin()) return -1;

		// loop while searching for the array end.
		while(!wr->isNext("]"))
		{

			if (!wr->readObjBegin()) return -1;

			const char* objectTypeStr=wr->readString();

			MirrorBase *mb=mirrorFactoryCreate(objectTypeStr);

			if (mb!=NULL)
			{
				mb->readJson(wr, this);

				// TODO we need to do this also:
				//mb->setupInternalRenderingData();
			}
			else
			{
				// Could not create the object. Probably an unknown type.
				printf("mirrorAdd failed on %s\n", objectTypeStr);
			}

			if (!wr->readObjEnd()) return -1;
		}

		// This ends the child objects array
		if (!wr->readArrayEnd()) return -1;
	}


	// Now we expect one more ']' to end the main array.
	if (!wr->readArrayEnd()) return -1;


	return 0;
}

#endif
#endif


