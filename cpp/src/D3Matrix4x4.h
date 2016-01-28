/*
D3Matrix4x4.h


www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
2015-03-09 Created by Henrik Bjorkman (www.eit.se/hb)

*/

#ifndef D3MATRIX4X4_H
#define D3MATRIX4X4_H

#include <SDL_opengl.h>

#include "D3Vector.h"
#include "D3PosAndOrientation.h"


/*
static const int cm00 = 0;  // ColumnMajor 0,0 
static const int cm01 = 1;  // ColumnMajor 0,1
static const int cm02 = 2;
static const int cm03 = 3;
static const int cm10 = 4;
static const int cm11 = 5;
static const int cm12 = 6;
static const int cm13 = 7;
static const int cm20 = 8;
static const int cm21 = 9;
static const int cm22 = 10;
static const int cm23 = 11;
static const int cm30 = 12;
static const int cm31 = 13;
static const int cm32 = 14;
static const int cm33 = 15;
*/

/*
enum
{
	cm00 = 0,  // ColumnMajor 0,0 
	cm01 = 1,  // ColumnMajor 0,1
	cm02 = 2,
	cm03 = 3,
	cm10 = 4,
	cm11 = 5,
	cm12 = 6,
	cm13 = 7,
	cm20 = 8,
	cm21 = 9,
	cm22 = 10,
	cm23 = 11,
	cm30 = 12,
	cm31 = 13,
	cm32 = 14,
	cm33 = 15
};
*/


class D3Matrix4x4 {

	public:

		// default constructor
		D3Matrix4x4();

		// copy constructor
		D3Matrix4x4(const D3Matrix4x4 &d);

		// assignment operator
		void operator=(const D3Matrix4x4 &d)
		{
			for (int i=0; i<16; i++)
			{
				matrix[i]=d.matrix[i];
			}
		}

		// Destructor is not needed, there are no allocated resources.
		// ~D3Vector();

		// Matrix from pos and orientation
		D3Matrix4x4(const D3PosAndOrientation &pao);


		//GLfloat matrix[16];
		float matrix[16];
};

#endif

