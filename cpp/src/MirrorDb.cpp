/*
MirrorDb.cpp

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

#include "MirrorDb.h"


MirrorDb::MirrorDb():
MirrorContainer(-1, -1, -1),
//idObjectsListSize(0),
//idObjectsList(NULL)
debug2(DEBUG2),
idObjectsVector(),
debug3(DEBUG3)
{
	// Initialize to zero (is this needed?) Don't think so since initial vector size is zero.
	for(size_t i=0; i<idObjectsVector.capacity();++i)
	{
		idObjectsVector[i]=NULL;
	}

};


MirrorDb::~MirrorDb()
{
	assert(debug2==DEBUG2 && debug3==DEBUG3);
	printf("~MirrorDb: %s\n", this->getName());
	if (idObjectsVector.size()!=0)
	{
		this->unlinkAllChildObjects();
	}
	debug2=0;
	debug3=0;
	for(size_t i=0; i<idObjectsVector.capacity();++i)
	{
		idObjectsVector[i]=NULL;
	}
}


int MirrorDb::findUnusedId()
{
	// First check if there is some unused slot in the current idObjectsVector.
	const size_t c=idObjectsVector.capacity();
	for(size_t i=0; i<c;++i)
	{
		if (idObjectsVector[i]==NULL)
		{
			return i;
		}
	}

	// There was not, idObjectsVector needs to be resized, give next id.
	return idObjectsVector.capacity();
}


void MirrorDb::registerIdObject(MirrorBase *childObject)
{
	int childId = childObject->getId();

	if (childId<0)
	{
		// All objects sent from server must have an ID but in local database we can allocate local IDs (just don't mix them)

		// Find an unused ID
		childId = findUnusedId();
		childObject->id=childId;

		// This is only for debugging, can be removed later.
		if (strcmp(this->getName(), "localDb")==0)
		{
			// OK, local objects can have locally generated IDs
			printf("%s: name=%s, id=~%d\n", this->getName(), childObject->getName(), childId);
		}
		else
		{
			printf("MirrorDb::registerIdObject: %s assigning local ID ~%d for object %s\n", this->getName(), childId, childObject->getName());
		}

	}


	while (childId>=idObjectsVector.capacity())
	{
		// Need a bigger list AKA vector

		const size_t s = idObjectsVector.capacity();
		if (s==0)
		{
			idObjectsVector.resize(1);
		}
		else
		{
			idObjectsVector.resize(s*2);
		}

		//childList = new MirrorBase*[childListSize]; // http://stackoverflow.com/questions/2265664/how-to-dynamically-allocate-the-array-of-pointers-in-c

		// Initialize to zero (is this needed?)
		for(int i=s; i<idObjectsVector.capacity();++i)
		{
			idObjectsVector[i]=NULL;
		}
	}

	// This is for debugging, check for errors and inconsistencies, eventually this shall not be needed when all is perfect.
	if (idObjectsVector[childId]!=NULL)
	{
		if (idObjectsVector[childId]==childObject)
		{
			// OK same object registered again.
			printf("MirrorDb::registerIdObject: reg again %d\n", childId);
		}
		else
		{
			// This shall not happen, must have forgotten to cleanup something.
			printf("MirrorDb::registerIdObject: not free %d\n", childId);
			//idObjectsVector[childId]->unlinkSelf();
			delete idObjectsVector[childId];
			idObjectsVector[childId]=NULL;
		}
	}

	idObjectsVector[childId]=childObject;

}


MirrorBase *MirrorDb::getObjectById(int objectId)
{
	if (objectId<0)
	{
		assert(objectId==-1);	// just for debugging
		return NULL;  // or shall we return this?
	}

	// just for debugging
	assert(objectId<idObjectsVector.capacity());

	MirrorBase *ptr=idObjectsVector[objectId];

	// All the is for debugging, it can be removed later when things work perfectly.
	if (ptr!=NULL)
	{
		assert(ptr->id == objectId);
		/*
		if(!(ptr->mirrorDb==this))
		{
			int cap=idObjectsVector.capacity();
			for(int i=0; i<cap; ++i)
			{
				MirrorBase *p=idObjectsVector[i];

				if (p!=NULL)
				{
					printf("%d %p\n", i, p);
				}
			}


			char* n=this->name;
			char *pn=ptr->name;
			char *pmn=ptr->mirrorDb->name;
			printf("why, n=%s, objectId=%d, pmn=%s, pn=%s, capacity=%d\n", n, objectId, pmn, pn, cap);
		}
		*/
		assert(ptr->mirrorDb==this);
	}

	return ptr;
}


void MirrorDb::unregisterIdObject(MirrorBase *mirrorObject)
{
	// just for debugging
	MIRROR_DEBUG_ASSERT();
	assert(debug2==DEBUG2 && debug3==DEBUG3);
	assert(mirrorObject!=NULL);

	const int id = mirrorObject->getId();

	// just for debugging
	assert(id>=0);
	assert(id<idObjectsVector.capacity());
	assert(idObjectsVector[id]==mirrorObject);
	assert(mirrorObject->mirrorDb==this);

	//printf("unregisterIdObject: db=%s, name=%s, id=~%d\n",this->getName(), mirrorObject->getName(), id);
	idObjectsVector[id]=NULL;

	// Perhaps caller should to do this because these two attributes belong there. For now caller will assert it is done here. So don't remove.
	mirrorObject->id=-1;
	mirrorObject->mirrorDb=NULL;

}
