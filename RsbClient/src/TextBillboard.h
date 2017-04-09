/*
 * TextBillboard.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: May 7, 2015
 *      Author: henrik
 */

#ifndef SRC_TextBillboard_H_
#define SRC_TextBillboard_H_


class DrawText;
class WindowObject;

class TextBillboard {
public:
	TextBillboard(WindowObject *windowObject, float width, float height);
	virtual ~TextBillboard();


	//virtual void renderAll();
	virtual void renderSelf();
	virtual void readSelf(WordReader *wr);
	#ifndef __EMSCRIPTEN__
	virtual void writeSelf(WordWriter *ww);
	#endif
	virtual void setupInternalRenderingData();

	virtual const char* getTypeName() const {return "TextBillboard";};

	void clearText();
	void setText(const char *str);

	WindowObject *windowObject;
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
	TextBillboard(const TextBillboard&);
	TextBillboard& operator=(const TextBillboard&);

};




#endif /* YUKIGASSENTEXTBILLBOARD_H_ */
