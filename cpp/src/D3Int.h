/*
 * D3Int.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 9, 2015
 *      Author: henrik
 */

#ifndef D3INT_H_
#define D3INT_H_

class WordReader;
class WordWriter;

class D3Int {
public:
	D3Int();
	D3Int(int x, int y ,int z):x(x),y(y),z(z){};

	virtual ~D3Int();

	bool equals(const D3Int &other)
	{
		return (this->x==other.x) && (this->y==other.y) && (this->z==other.z);
	}

	// compare equals operator
	bool operator==(const D3Int &v)
	{
		return (this->x == v.x) && (this->y == v.y) && (this->z == v.z);
	}


	void readSelf(WordReader *wr);
	void writeSelf(WordWriter *ww);


	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
protected:
	D3Int(const D3Int&);
	D3Int& operator=(const D3Int&);

public:
	int x;
	int y;
	int z;

};

#endif /* D3INT_H_ */
