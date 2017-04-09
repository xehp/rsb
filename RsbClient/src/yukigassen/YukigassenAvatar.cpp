/*
YukigassenAvatar.cpp

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include <assert.h>
#include "includeGlEtc.h"
#include "CreateProgram.h"
#include "HybridLib.h"
#include "WordReader.h"
#ifndef __EMSCRIPTEN__
#include "WordWriter.h"
#endif
#include "MirrorIdList.h"
#include "OrtogonalBox.h"
#include "RsbLong.h"
#include "YukigassenInventory.h"
#include "TextBillboard.h"
#include "YukigassenRoom.h"
#include "YukigassenAvatar.h"

const float YukigassenAvatar::headSize = 0.4f;  // This should have been distance from eyes to feet
const float YukigassenAvatar::bodyLength = 0.6f;
const float YukigassenAvatar::noseToFeetLength = 1.3f; // This constant shall have same value as noseToFeetLength in YukigassenAvatar.java

YukigassenAvatar::YukigassenAvatar() :
	YukigassenRoom(),
	//allowedMove(),
	objectTime(0),
	avatarStateMs(0),
	avatarAmmo(0),
	avatarPlayerReferencesId(-1),
	serverChangedPositionCounter(0)
	//teamIndex(0),
	//head1(NULL),
	//head2(NULL)
	,nameTag(NULL)
	,inv(NULL)
{
	// TODO: Avatar should be made up of a head and a body.
	// Those two objects shall then be rendered instead of this object.
	// For now creating the head here.

	/*
	head1=new YukigassenTexturedRoom();
	head1->textureIndex=1;
	head1->roomSize.set(0.35f,0.35f,headSize);
	head2=new YukigassenTexturedRoom();
	head2->textureIndex=2;
	head2->roomSize.set(0.35f,0.35f,headSize);
	*/
	//body=new YukigassenTexturedRoom();
	//body->roomSize.set(0.35f,0.5f,bodyLength);

	//nameTag = new YukigassenTextBillboard(1.0f,0.25f);
	//nameTag->setText(this->getName());
	//nameTag->linkSelf(this);
	// Had a problem here, we can not link the nameTag since it might interfere with objects sent from server. But it needs to be linked so that it can get WindowsObject. Either me change server so that it adds a dummy for this or we create one more class for billboards that does not need to be linked. Or we put this object in a local db where it can be linked and refer to it from here.
}


YukigassenAvatar::~YukigassenAvatar()
{
	//delete head1;
	//delete head2;
	//delete body;
	delete nameTag;
	inv=NULL; // this object is deleted with the other child objects, not here.
}



void YukigassenAvatar::readSelf(WordReader *wr)
{
	YukigassenRoom::readSelf(wr);

	//allowedMove.readSelf(wr);
	objectTime = wr->readLong();
	avatarStateMs = wr->readInt();
	avatarAmmo = wr->readInt();
	avatarPlayerReferencesId = wr->readInt();
	serverChangedPositionCounter = wr->readInt();
}

#ifndef __EMSCRIPTEN__
void YukigassenAvatar::writeSelf(WordWriter *ww)
{
	YukigassenRoom::writeSelf(ww);

	//allowedMove.readSelf(wr);
	ww->writeLong(objectTime);
	ww->writeInt(avatarStateMs);
	ww->writeInt(avatarAmmo);
	ww->writeInt(avatarPlayerReferencesId);
	ww->writeInt(serverChangedPositionCounter);
}
#endif


/*
void YukigassenAvatar::limitPos(D3Vector &pos)
{
	if (pos.x > allowedMove.ux)
	{
		pos.x = allowedMove.ux;
	}
	else if (pos.x < allowedMove.lx)
	{
		pos.x = allowedMove.lx;
	}

	if (pos.y > allowedMove.uy)
	{
		pos.y = allowedMove.uy;
	}
	else if (pos.y < allowedMove.ly)
	{
		pos.y = allowedMove.ly;
	}

	if (pos.z > allowedMove.uz)
	{
		pos.z = allowedMove.uz;
	}
	else if (pos.z < allowedMove.lz)
	{
		pos.z = allowedMove.lz;
	}
}
*/

void YukigassenAvatar::renderAll()
{
	if (this->avatarStateMs>0)
	{
		int f = this->avatarStateMs / 250;
		if (f%2==0)
		{
			return;
		}
	}

	if (this->id==ourAvatarId)
	{
		return;
	}

	YukigassenRoom::renderAll();
}


void YukigassenAvatar::renderSelf()
{
	// Read more at: https://gitlab.com/wikibooks-opengl/modern-tutorials/blob/master/tut06_textures/cube.cpp



	// Head shall probably be its own ID object and sent from server, but for now it is handled locally.
	/*
	switch(this->teamIndex)
	{
		case 1: if (head1!=NULL) {head1->renderSelf();} break;
		case 2: if (head2!=NULL) {head2->renderSelf();} break;
		default:
			break;
	}
	*/

	glm::mat4 orgMVP(MVP);


	// The MVP is already set for coordinates of this object. So to get a position some distance in below this just add the distance to Z (no need to use head vector). Or is it the other way around?
	/*glm::mat4 bodyModel = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			//-pao.head.x*0.5f, -pao.head.y*0.5f, -pao.head.z*0.5f, 1.0f
			0.0f, 0.0f, 0.2f+ bodyLength/2, 1.0f
	);
	MVP = orgMVP * bodyModel; // Remember, matrix multiplication is the other way around
	body->renderSelf();*/


	// The MVP is already set for coordinates of this object. So to get a position some distance in below this just add the distance to Z (no need to use head vector). Or is it the other way around?
	glm::mat4 tagModel = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			//-pao.head.x*0.5f, -pao.head.y*0.5f, -pao.head.z*0.5f, 1.0f
			0.0f, 0.0f, -0.5f, 1.0f
	);

	MVP = orgMVP * tagModel;

	assert(nameTag!=NULL);
	nameTag->renderSelf();

	MVP = orgMVP;
}


void YukigassenAvatar::setupInternalRenderingData()
{
	/*AvatarPlayerReferences *apr=dynamic_cast<AvatarPlayerReferences *>(mirrorDb->getObjectById(avatarPlayerReferencesId));
	if (apr!=NULL)
	{
		apr->teamIndex = teamIndex;
	}*/

	assert(idListPtr!=NULL);

	MirrorBase *mb = idListPtr->getObjectById(avatarPlayerReferencesId);
	if (mb!=NULL)
	{
		mb->dataChanged();
	}


	//if (head1!=NULL) {head1->setupInternalRenderingData();}
	//if (head2!=NULL) {head2->setupInternalRenderingData();}
	//body->setupInternalRenderingData();

	assert(nameTag!=NULL);
	nameTag->setText(this->getName());
	nameTag->setupInternalRenderingData();
}

void YukigassenAvatar::regNamedChildObject(MirrorBase *addedObject)
{
	YukigassenInventory *rl=dynamic_cast<YukigassenInventory *>(addedObject);

	if ( (rl!=NULL) && ( strcmp(rl->getName(), "_inv")==0) )
	{
		assert((inv==NULL) || (inv=rl));
		inv=rl;
	}
	else
	{
		// Perhaps super class knows what to do with this one?
		YukigassenRoom::regNamedChildObject(addedObject);
	}

}

void YukigassenAvatar::linkSelf(MirrorBase *parentPtr)
{
	YukigassenRoom::linkSelf(parentPtr);
	WindowObject *windowObject=getWindowObject();
	assert(windowObject!=NULL);
	nameTag = new TextBillboard(windowObject, 1.0f,0.25f);
}


