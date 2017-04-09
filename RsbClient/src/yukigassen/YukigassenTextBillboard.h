/*
 * YukigassenTextBillboard.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: May 7, 2015
 *      Author: henrik
 */

#ifndef YUKIGASSENTEXTBILLBOARD_H_
#define YUKIGASSENTEXTBILLBOARD_H_

#include "YukigassenProperty.h"

class DrawText;

class YukigassenTextBillboard : public YukigassenProperty {
public:
	YukigassenTextBillboard(float width, float height);
	virtual ~YukigassenTextBillboard();


	//virtual void renderAll();
	virtual void renderSelf();
	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif
	virtual void setupInternalRenderingData();

	virtual const char* getTypeName() const {return "YukigassenTextBillboard";};

	void clearText();
	void setText(const char *str);

	DrawText *drawText;
	float width;
	float height;


	GLuint vbo_vertices;
	GLuint ibo_elements;
	GLuint vbo_texcoords;
	GLuint texture_id; // The GL texture

	int textureIndex; // Which part of the texture to use
	int entityColor;


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	YukigassenTextBillboard(const YukigassenTextBillboard&);
	YukigassenTextBillboard& operator=(const YukigassenTextBillboard&);

};




#endif /* YUKIGASSENTEXTBILLBOARD_H_ */
