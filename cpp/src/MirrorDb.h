/*
MirrorDb.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef MirrorDb_H
#define MirrorDb_H

#include <vector>
#include "MirrorContainer.h"

#define DEBUG2 177878654
#define DEBUG3 98531368

class MirrorDb : public MirrorContainer
{
public:
	MirrorDb();

	virtual ~MirrorDb();

	//virtual void renderSelf();

	//virtual void addObject(MirrorBase *mirrorObject);

	void registerIdObject(MirrorBase *mirrorObject);
	MirrorBase *getObjectById(int objectId);
	void unregisterIdObject(MirrorBase *mirrorObject);
	int findUnusedId();


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	MirrorDb(const MirrorDb&);
	MirrorDb& operator=(const MirrorDb&);

	int debug2;
	//int idObjectsListSize;
	//MirrorBase **idObjectsList; // TODO use std::vector
	std::vector<MirrorBase *> idObjectsVector;
	int debug3;
};

#endif
