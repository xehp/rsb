/*
D3Vector.java


www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license


History:
2015-03-09
Created by Henrik Bjorkman (www.eit.se/hb) by translating own D3Vector.java to c++.

*/




#ifndef D3VECTOR_H
#define D3VECTOR_H


//#include <SDL_opengl.h>

		// In OpenGL arrays are used to store 4x4 matrixes in "column-major order".
		// The matrix is stored in the array like this:
		//
		//        0,0  1,0  2,0  3,0
		//
		//        0,1  1,1  2,1  3,1
		//  M = [                    ]
		//        0,2  1,2  2,2  3,2
		//
		//        0,3  1,3  2,3  3,3
		//
		// Here are indexes to use to access each matrix position.
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

		//
		//
		// But we are used to row-major, it looks as below.
		//
		//        0,0  0,1  0,2  0,3
		//
		//        1,0  1,1  1,2  1,3
		//  M = [                    ]
		//        2,0  2,1  2,2  2,3
		//
		//        3,0  3,1  3,2  3,3
		//
		// Here are translation indexes:
		enum
		{
			rm00 = cm00, // RowMajor 0,0
			rm01 = cm10, // RowMajor 0,1 is same as ColumnMajor 1,0
			rm02 = cm20,
			rm03 = cm30,
			rm10 = cm01,
			rm11 = cm11,
			rm12 = cm21,
			rm13 = cm31,
			rm20 = cm02,
			rm21 = cm12,
			rm22 = cm22,
			rm23 = cm32,
			rm30 = cm03,
			rm31 = cm13,
			rm32 = cm23,
			rm33 = cm33
		};


		//
		// Or if its preferred to count positions from 1
		//
		//        1,1  1,2  1,3  1,4
		//
		//        2,1  2,2  2,3  2,4
		//  M = [                    ]
		//        3,1  3,2  3,3  3,4
		//
		//        4,1  4,2  4,3  4,4
		enum
		{
			a11=rm00,
			a12=rm01,
			a13=rm02,
			a14=rm03,
			a21=rm10,
			a22=rm11,
			a23=rm12,
			a24=rm13,
			a31=rm20,
			a32=rm21,
			a33=rm22,
			a34=rm23,
			a41=rm30,
			a42=rm31,
			a43=rm32,
			a44=rm33
		};



class WordReader;
class WordWriter;


class D3Vector {

	public:


		// default constructor
		D3Vector();

		// copy constructor
		D3Vector(const D3Vector &v);

		// assignment operator
		void operator=(const D3Vector &v)
		{
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
		}

		// compare equals operator
		bool operator==(const D3Vector &v)
		{
			return (this->x == v.x) && (this->y == v.y) && (this->z == v.z);
		}


		// Destructor is not needed, there are no allocated resources.
		// ~D3Vector();

		// a normal constructor
		D3Vector(float x, float y, float z);

		// Vector is created as the cross of v1 and v2
		D3Vector(const D3Vector &v1, const D3Vector &v2);

		// Vector is created as the sum of of v1 and scale*v2
		D3Vector(const D3Vector &v1, const float scale, const D3Vector &v2);

		// Set this vector to the cross of v1 and v2
		void cross(const D3Vector &v1, const D3Vector &v2);

		// https://en.wikipedia.org/wiki/Dot_product
		float dot(const D3Vector &v);

		void add(const D3Vector &v);

		void add(float scale, const D3Vector &v);

		void sub(const D3Vector &v);

		void set(float x, float y, float z) {this->x=x;this->y=y;this->z=z;};

		void set(const D3Vector &v) {this->x=v.x;this->y=v.y;this->z=v.z;};

		void scale(const float f);

		void invScale(const float f);

		// absolute length of the vector
		float absLength() const;

		void normalize();

		bool equals(const D3Vector &v);

		void readSelf(WordReader *wr);
		void writeSelf(WordWriter *ww);
		
		void toArray4(float a[]);
		void MulMatrix4x4(const float m[]);
		

	public:
		/*GLfloat*/float x;
		/*GLfloat*/float y;
		/*GLfloat*/float z;
};

#endif


