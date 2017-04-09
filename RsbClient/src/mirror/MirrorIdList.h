/*
 * MirrorIdList.h
 *
 *  Created on: Mar 25, 2016
 *      Author: henrik
 */

#ifndef MIRRORIDLIST_H_
#define MIRRORIDLIST_H_

//namespace ServerLib {
#include "MirrorContainer.h"



class MirrorIdList : public MirrorContainer {
	enum{
		debugHelpMagicNumber=876522
	};

public:
	MirrorIdList();
	virtual ~MirrorIdList();

	virtual void linkSelf(MirrorBase *newParent);
	#ifdef JSON_READER
	int readJson(WordReader *wr, MirrorBase *parent);
	#endif
	void registerIdObject(MirrorBase *idObject);
	MirrorBase *getObjectById(int objectId);
	void unregisterIdObject(MirrorBase *mirrorObject);
	int findUnusedId();
	//int getNIdObjects() const {return nIdObjects;}; // currently this is not used so commented out.

private:

	int debugHelp;
	std::vector<MirrorBase *> idObjectsVector;

	int nIdObjects;
};

//} /* namespace ServerLib */

#endif /* MIRRORIDLIST_H_ */
