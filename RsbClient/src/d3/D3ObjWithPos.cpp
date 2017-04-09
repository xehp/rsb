/*
D3ObjWithPos.cpp

Copyright (C) 2015 Henrik Björkman www.eit.se/hb

History:
Created 2015 by Henrik Bjorkman www.eit.se



www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

*/


#include "includeGlEtc.h"
#include "HybridLib.h"
#include "D3ObjWithPos.h"


#ifndef RSB_SERVER
#define GET_TICKS() SDL_GetTicks()
#else
#define GET_TICKS() 0
#endif



D3ObjWithPos::D3ObjWithPos() :
	MirrorContainer(),
	pao(),
	objectVelocity(),
	//gameTimeOfLastUpdate()
	tickOfLastUpdateMs(GET_TICKS())
{



}

D3ObjWithPos::~D3ObjWithPos()
{}


void D3ObjWithPos::readSelf(WordReader *wr)
{
	MirrorContainer::readSelf(wr);
	pao.readSelf(wr);
	objectVelocity.readSelf(wr);

	// Get the time of this world that the update is for. Can be used to extrapolate the position.
	// TODO The server should keep that info.
	///gameTimeOfLastUpdate = this->mirrorDb->getWorld()->getGameTime();  // or shall we just use SDL_GetTicks()?
	// Here we only need the system ticks.
	tickOfLastUpdateMs = GET_TICKS();
}

#ifndef __EMSCRIPTEN__
void D3ObjWithPos::writeSelf(WordWriter *ww)
{
	MirrorContainer::writeSelf(ww);
	pao.writeSelf(ww);
	objectVelocity.writeSelf(ww);

	// Get the time of this world that the update is for. Can be used to extrapolate the position.
	// TODO The server should keep that info.
	///gameTimeOfLastUpdate = this->mirrorDb->getWorld()->getGameTime();  // or shall we just use SDL_GetTicks()?
	// Here we only need the system ticks.
	tickOfLastUpdateMs = GET_TICKS();
}
#endif

#ifndef RSB_SERVER
void D3ObjWithPos::renderAll()
{
	// experiment with extrapolation
	const int currentTickMs = GET_TICKS();
	const int deltaTimeMs = (currentTickMs - tickOfLastUpdateMs);
	pao.pos.add(deltaTimeMs*0.001f, objectVelocity);
	tickOfLastUpdateMs = currentTickMs;


	glm::mat4 savedMVP = MVP;


	// translation matrix
	glm::mat4 cubeModelT = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			pao.pos.x, pao.pos.y, pao.pos.z, 1.0f
	);

	// right vector as cross of nose and head
	D3Vector rightSide(pao.nose, pao.head);



	// Rotation matrix
	glm::mat4 cubeModelR = glm::mat4(
		 	pao.nose.x,   pao.nose.y,  pao.nose.z,  0.0f,
			rightSide.x, rightSide.y, rightSide.z, 0.0f,
			-pao.head.x, -pao.head.y, -pao.head.z,  0.0f,
		         0.0f,             0.0f,       0.0f, 1.0f
	);


	glm::mat4 cubeModel = cubeModelT * cubeModelR;


	MVP = MVP * cubeModel;


	// MVP is now set up for rendering this object and its child objects. Call base class renderAll.
	MirrorContainer::renderAll();

	// Restore MVP matrix.
	MVP = savedMVP;

}
#endif


// Translates from coordinates of this room to global
void D3ObjWithPos::translatePaoToGlobal(D3PosAndOrientation &pao) const
{
	// If this is not specialized/override by a sub/extending class then there is no more translating to do.
	return;
}


