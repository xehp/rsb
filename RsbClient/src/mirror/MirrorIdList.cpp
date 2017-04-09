/*
 * MirrorIdList.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: henrik
 */

#include "MirrorIdList.h"



/*
MirrorIdList.cpp

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

#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#ifdef JSON_READER
#include "MirrorFactory.h"
#include "WordReader.h"
#endif
#endif
#include "MirrorIdList.h"

MirrorIdList::MirrorIdList():
MirrorContainer(),
debugHelp(debugHelpMagicNumber),
idObjectsVector(256),
nIdObjects(0)
{
	// TODO the loop below should not be need, std::vector should initialize all the pointers to 0. But keeping it for now.
	for(size_t i=0; i<idObjectsVector.size();++i)
	{
		idObjectsVector[i]=NULL;
	}

	// IdList shall be object ~0 in its own list.
	id=0;
};

MirrorIdList::~MirrorIdList()
{
	assert(debugHelp==debugHelpMagicNumber);
	printf("~MirrorIdList: %s\n", this->getName());
	assert(nIdObjects<=idObjectsVector.size());

/*
	if (idObjectsVector.size()!=0)
	{
		this->unlinkAllChildObjects();
	}
*/

	// I would expect all objects to have been deleted by MirrorContainer
	// before getting here but unregister any ones that remain anyway.
	for(size_t i=0; i<idObjectsVector.size();++i)
	{
		MirrorBase *mv=idObjectsVector[i];
		if (mv!=NULL)
		{
			printf("MirrorIdList::~MirrorIdList: delete %.32s\n", mv->getName());
			unregisterIdObject(mv);
		}
	}

	assert(nIdObjects==0);
}


int MirrorIdList::findUnusedId()
{
	// First check if there is some unused slot in the current idObjectsVector.
	const size_t c=idObjectsVector.size();
	for(size_t i=0; i<c;++i)
	{
		if (idObjectsVector[i]==NULL)
		{
			return i;
		}
	}

	// There was not, idObjectsVector needs to be resized, give next id.
	return idObjectsVector.size();
}

// This registers an object in the ID List.
// This will not do all other linking stuff, for that use linkSelf (which will call this among other things).
void MirrorIdList::registerIdObject(MirrorBase *idObject)
{
	int childId = idObject->getId();

	// Does the object to add have an ID already or not?
	if (childId<0)
	{
		// No, the object did not have an ID.
		// All objects sent from server must have an ID but in local database we can allocate local IDs (just don't mix them)

		// Find an unused ID
		childId = findUnusedId();
		idObject->id=childId;

		// This is only for debugging, can be removed later.
		if (strcmp(this->getName(), "localDb")==0)
		{
			// OK, local objects can have locally generated IDs
			//printf("%s: name=%s, id=~%d\n", this->getName(), idObject->getName(), childId);
		}
		else
		{
			printf("MirrorIdList::registerIdObject: %s assigning local ID ~%d for object %s\n", this->getName(), childId, idObject->getName());
			fflush(stdout);
		}

	}

	// Is our vector big enough?
	while (childId>=idObjectsVector.size())
	{
		// Need a bigger list AKA vector

		// TODO in std::vector there is a built in method for doing this. Called reserve.

		const size_t ivc = idObjectsVector.size();
		if (ivc==0)
		{
			idObjectsVector.resize(1);
		}
		else
		{
			idObjectsVector.resize(ivc*2);
		}

		//childList = new MirrorBase*[childListSize]; // http://stackoverflow.com/questions/2265664/how-to-dynamically-allocate-the-array-of-pointers-in-c

		// Initialize to zero (is this needed?)
		for(int i=ivc; i<idObjectsVector.size();++i)
		{
			idObjectsVector[i]=NULL;
		}
	}

	// This is for debugging, check for errors and inconsistencies, eventually this shall not be needed when all is perfect.
	if (idObjectsVector[childId]!=NULL)
	{
		if (idObjectsVector[childId]==idObject)
		{
			// OK same object registered again.
			printf("MirrorIdList::registerIdObject: reg again %d\n", childId);
		}
		else
		{
			// This shall not happen, must have forgotten to cleanup something.
			printf("MirrorIdList::registerIdObject: not free %d\n", childId);
			fflush(stdout);
			//idObjectsVector[childId]->unlinkSelf();
			delete idObjectsVector[childId];
			idObjectsVector[childId]=NULL;
		}
	}

	idObjectsVector[childId]=idObject;
	idObject->idListPtr=this;
	assert(idObject->id==childId);

	nIdObjects++;

	// just for debugging
	assert(nIdObjects<=idObjectsVector.size());
}


MirrorBase *MirrorIdList::getObjectById(int objectId)
{
	assert(debugHelp==debugHelpMagicNumber);

	if (objectId<0)
	{
		assert(objectId==-1);	// just for debugging
		return NULL;  // or shall we return this?
	}

	if (objectId>=idObjectsVector.size())
	{
		return NULL;
	}

	MirrorBase *ptr=idObjectsVector[objectId];

	// This is for debugging, it can be removed or commented out later when things work perfectly.
	if (ptr!=NULL)
	{
		assert(ptr->id == objectId);
		assert(ptr->idListPtr==this);
	}

	return ptr;
}


void MirrorIdList::unregisterIdObject(MirrorBase *mirrorObject)
{
	// just for debugging
	assert(mirrorObject!=NULL);
	assert(mirrorObject->idListPtr==this);

	const int id = mirrorObject->getId();

	// just for debugging
	assert(id>=0);
	assert(id<idObjectsVector.size());
	//assert(idObjectsVector[id]==mirrorObject);
	//assert(mirrorObject->idListPtr==this);

	//printf("unregisterIdObject: db=%s, name=%s, id=~%d\n",this->getName(), mirrorObject->getName(), id);
	idObjectsVector[id]=NULL;
	mirrorObject->id=-1;
	mirrorObject->idListPtr=NULL;

	nIdObjects--;

	// just for debugging
	assert(nIdObjects<=idObjectsVector.size());

}


void MirrorIdList::linkSelf(MirrorBase *newParent)
{
	MirrorContainer::linkSelf(newParent);

	// Register self in own ID list.
	idListPtr = this;
	assert(id == 0);
	registerIdObject(this);
}

#ifdef JSON_READER

// There is a small problem when reading JSON
// The top most object may have had one index when it was saved but that place can now be used for some other object.
// Probably the correct solution is wo save it with index -1 but for now we do "this->index = -1;" here.
// Return a negative value if not OK
int MirrorIdList::readJson(WordReader *wr, MirrorBase *parent)
{
	// First we expect a ':' character.
	if (!wr->readObjSeparator()) return -1;

	// We expect now a main array with one or two arrays inside.
	if (!wr->readArrayBegin()) return -1;

	// This begins the first array (the one with objects own data)
	if (!wr->readArrayBegin()) return -1;

	// It contains all data about this object-
	readSelf(wr);

	// Index fix: In the server this object may have an index that is taken already
	// Apart from this fix, this code does the same as in MirrorContainer.
	this->index = -1;

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
