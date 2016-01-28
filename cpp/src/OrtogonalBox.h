/*
 * OrtogonalBox.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 14, 2015
 *      Author: henrik
 */

#ifndef ORTOGONALBOX_H_
#define ORTOGONALBOX_H_

class WordReader;
class WordWriter;

class OrtogonalBox {
public:
	OrtogonalBox();
	virtual ~OrtogonalBox();


	float ux;
	float lx;
	float uy;
	float ly;
	float uz;
	float lz;

	void readSelf(WordReader *wr);
	void writeSelf(WordWriter *ww);


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	OrtogonalBox(const OrtogonalBox&);
	OrtogonalBox& operator=(const OrtogonalBox&);

};

#endif /* ORTOGONALBOX_H_ */
