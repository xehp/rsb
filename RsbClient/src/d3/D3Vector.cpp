/*
D3Vector.java



www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license



History:
2015-03-09
Created by Henrik Bjorkman (www.eit.se/hb) by translating own D3Vector.java to c++.

*/

#include <math.h>       // needed for sqrt 
#ifndef RSB_SERVER
#include <SDL_opengl.h>
#endif
#include "WordReader.h"
#include "WordWriter.h"
#include "D3Vector.h"


/*
static const int a11=D3PosAndOrientation.rm00;
static const int a12=D3PosAndOrientation.rm01;
static const int a13=D3PosAndOrientation.rm02;
static const int a14=D3PosAndOrientation.rm03;
static const int a21=D3PosAndOrientation.rm10;
static const int a22=D3PosAndOrientation.rm11;
static const int a23=D3PosAndOrientation.rm12;
static const int a24=D3PosAndOrientation.rm13;
static const int a31=D3PosAndOrientation.rm20;
static const int a32=D3PosAndOrientation.rm21;
static const int a33=D3PosAndOrientation.rm22;
static const int a34=D3PosAndOrientation.rm23;
static const int a41=D3PosAndOrientation.rm30;
static const int a42=D3PosAndOrientation.rm31;
static const int a43=D3PosAndOrientation.rm32;
static const int a44=D3PosAndOrientation.rm33;
*/



// Default constructor
D3Vector::D3Vector() : x(0), y(0), z(0)
{
}


D3Vector::D3Vector(float x, float y, float z)  : x(x), y(y), z(z)
{
}


// Copy constructor
D3Vector::D3Vector(const D3Vector &v) : x(v.x), y(v.y), z(v.z)  
{
}


// Vector is created as the cross of v1 and v2
D3Vector::D3Vector(const D3Vector &v1, const D3Vector &v2)
{
	this->x = v1.y*v2.z - v1.z*v2.y;
	this->y = v1.z*v2.x - v1.x*v2.z;
	this->z = v1.x*v2.y - v1.y*v2.x;
}


// Vector is created the sum of of v1 and scale*v2
D3Vector::D3Vector(const D3Vector &v1, const float scale, const D3Vector &v2)
{
	this->x = v1.x+scale*v2.x;
	this->y = v1.y+scale*v2.y;
	this->z = v1.z+scale*v2.z;
}


// this vector is the cross of v1 and v2
void D3Vector::cross(const D3Vector &v1, const D3Vector &v2)
{
	this->x = v1.y*v2.z - v1.z*v2.y;
	this->y = v1.z*v2.x - v1.x*v2.z;
	this->z = v1.x*v2.y - v1.y*v2.x;
}


// https://en.wikipedia.org/wiki/Dot_product
float D3Vector::dot(const D3Vector &v)
{
	return this->x * v.x + this->y * v.y + this->z * v.z;
}


void D3Vector::add(const D3Vector &v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
}


void D3Vector::add(float scale, const D3Vector &v)
{
	this->x += scale*v.x;
	this->y += scale*v.y;
	this->z += scale*v.z;
}


void D3Vector::sub(const D3Vector &v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
}


void D3Vector::scale(const float f)
{		
	x*=f;
	y*=f;
	z*=f;
}


void D3Vector::invScale(const float f)
{		
	x/=f;
	y/=f;
	z/=f;
}


// absolute length of the vector
float D3Vector::absLength() const
{
	return (float) sqrt(x*x+y*y+z*z);
}


void D3Vector::normalize()
{
	invScale(absLength());
}


static bool D3Vector_equals(const float f1, const float f2, const float tolerance)
{
	float d=f1-f2;
	if ((d>tolerance) || (d<-tolerance))
	{
		return false;
	}
	return true;
} 


bool D3Vector::equals(const D3Vector &v)
{		
	const float tolerance = absLength()/10000;

	if (!D3Vector_equals(x, v.x, tolerance))
	{
		return false;
	}
	if (!D3Vector_equals(y, v.y, tolerance))
	{
		return false;
	}
	if (!D3Vector_equals(z, v.z, tolerance))
	{
		return false;
	}
			
	return true;
}


#if 0
float[] D3Vector::toArray4()
{
	const float a[]={x, y, z, 1.0f};
	return a;
}


D3Vector::D3Vector(const float a[])
{
	x=a[0];
	y=a[1];
	z=a[2];
}

// Multiply a 4x4 matrix with the vector
// The matrix shall be stored OpenGL style.
void D3Vector::MulMatrix4x4(const float m[])
{
	const float v[]=toArray4();
			
	x=m[a11]*v[0]+m[a12]*v[1]+m[a13]*v[2]+m[a14]*v[3];
	y=m[a21]*v[0]+m[a22]*v[1]+m[a23]*v[2]+m[a24]*v[3];
	z=m[a31]*v[0]+m[a32]*v[1]+m[a33]*v[2]+m[a34]*v[3];
	//=m[a41]*v[0]+m[a42]*v[1]+m[a43]*v[2]+m[a44]*v[3];
}

// Create a new vector as the result of a Matrix4x4 multiplied with a Vector4
// Multiply a 4x4 matrix with the vector
// The matrix shall be stored OpenGL style.
D3Vector::D3Vector(const float m[], const float v[])
{
	x=m[a11]*v[0]+m[a12]*v[1]+m[a13]*v[2]+m[a14]*v[3];
	y=m[a21]*v[0]+m[a22]*v[1]+m[a23]*v[2]+m[a24]*v[3];
	z=m[a31]*v[0]+m[a32]*v[1]+m[a33]*v[2]+m[a34]*v[3];
	//=m[a41]*v[0]+m[a42]*v[1]+m[a43]*v[2]+m[a44]*v[3];
}


// Calculate the distance between two positions (given by vectors a and b)
static float D3Vector::distance(const D3Vector a, const D3Vector b)
{
	D3Vector c=new D3Vector(a);
	c.sub(b);
	float d=c.absLength();
	delete c;
	return d;
}
#endif


void D3Vector::readSelf(WordReader *wr)
{
    if ((wr->equalsIgnoreCase("-")) || (wr->equalsIgnoreCase("null")))
    {
    	wr->readToken();
    	x=0;
    	y=0;
    	z=0;
    }
    else
    {
		x=wr->readFloat();
		y=wr->readFloat();
		z=wr->readFloat();
    }
}

void D3Vector::writeSelf(WordWriter *ww)
{
	ww->writeFloat(x);
	ww->writeFloat(y);
	ww->writeFloat(z);
}


// A must be a vector with 4 floats.
void D3Vector::toArray4(float a[])
	{
		a[0]=x;
		a[1]=y;
		a[2]=z;
		a[3]=1.0f;
	}



// Multiply a 4x4 matrix with the vector
// The matrix shall be stored OpenGL style.
void D3Vector::MulMatrix4x4(const float m[])
{
	float v[4];
	toArray4(v);
			
	x=m[a11]*v[0]+m[a12]*v[1]+m[a13]*v[2]+m[a14]*v[3];
	y=m[a21]*v[0]+m[a22]*v[1]+m[a23]*v[2]+m[a24]*v[3];
	z=m[a31]*v[0]+m[a32]*v[1]+m[a33]*v[2]+m[a34]*v[3];
	//=m[a41]*v[0]+m[a42]*v[1]+m[a43]*v[2]+m[a44]*v[3];
}


