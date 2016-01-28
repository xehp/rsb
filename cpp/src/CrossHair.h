/*
 * CrossHair.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Jul 22, 2015
 *      Author: drekkar
 */

#ifndef CROSSHAIR_H_
#define CROSSHAIR_H_

//#include "includeGlEtc.h"

#include "MirrorContainer.h"


class CrossHair : public MirrorContainer{
public:
	CrossHair(float width, float height);
	virtual ~CrossHair();


	virtual void renderAll();

	GLuint crossVBO;
	GLuint crossIBO;



	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	CrossHair(const CrossHair&);
	CrossHair& operator=(const CrossHair&);



};

#endif /* CROSSHAIR_H_ */
