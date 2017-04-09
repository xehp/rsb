/*
 * YukigassenTexturedRoom.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 15, 2015
 *      Author: henrik
 */

#ifndef YUKIGASSENTEXTUREDROOM_H_
#define YUKIGASSENTEXTUREDROOM_H_

#include <SDL_opengl.h>
#include "YukigassenRoom.h"


class YukigassenTexturedRoom : public YukigassenRoom {

public:
	YukigassenTexturedRoom();
	virtual ~YukigassenTexturedRoom();


	//virtual void renderAll();
	virtual void renderSelf();
	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif
	virtual void setupInternalRenderingData();
	virtual const char* getTypeName() const {return "YukigassenTexturedRoom";};

	GLuint vbo_vertices;
	GLuint ibo_elements;
	GLuint vbo_texcoords;
	static GLuint texture_id; // The GL texture

	int textureIndex; // Which part of the image/texture "townhall.png" to use, all texture data are stored in one image, this index tells from which part of the image the texture shall be made.
	int prevTextureIndex;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenTexturedRoom(const YukigassenTexturedRoom&);
	YukigassenTexturedRoom& operator=(const YukigassenTexturedRoom&);


};


void YukigassenTexturedRoomCleanup();

#endif /* YUKIGASSENTEXTUREDROOM_H_ */
