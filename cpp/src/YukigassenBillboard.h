/*
 * YukigassenBillboard.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: May 04, 2015
 *      Author: henrik
 */

#ifndef YUKIGASSENBILLBOARD_H_
#define YUKIGASSENBILLBOARD_H_

#include "YukigassenProperty.h"
class YukigassenBillboard : public YukigassenProperty {
public:
	YukigassenBillboard();
	virtual ~YukigassenBillboard();


	//virtual void renderAll();
	virtual void renderSelf();
	virtual void readSelf(WordReader *wr);
	virtual void setupInternalRenderingData();

	GLuint vbo_vertices;
	GLuint ibo_elements;
	GLuint vbo_texcoords;
	static GLuint texture_id; // The GL texture

	int textureIndex; // Which part of the texture to use
	//int prevTextureIndex;
	int entityColor;

	virtual const char* getTypeName() const {return "YukigassenBillboard";};


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenBillboard(const YukigassenBillboard&);
	YukigassenBillboard& operator=(const YukigassenBillboard&);

};


#endif /* YUKIGASSENBILLBOARD_H_ */
