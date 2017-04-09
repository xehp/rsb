/*
D3PosAndOrientation.cpp




www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license



History:
2015-03-09
Created by Henrik Bjorkman (www.eit.se/hb) by translating own D3PosAndOrientation.java to c++.

*/

#include <string.h>
#include "WordReader.h"
#include "WordWriter.h"

#include "D3Vector.h"
#include "D3PosAndOrientation.h"

// Default constructor
D3PosAndOrientation::D3PosAndOrientation() : pos(0,0,0), nose(1,0,0), head(0,0,-1)
{
}

// Copy constructor
D3PosAndOrientation::D3PosAndOrientation(const D3PosAndOrientation &pao) : pos(pao.pos), nose(pao.nose), head(pao.head)  
{
}


D3PosAndOrientation::D3PosAndOrientation(const D3Vector &pos, const D3Vector &nose, const D3Vector &head) : pos(pos), nose(nose), head(head)
{
}

// This will keep nose and make sure head is perpendicular to it.
void D3PosAndOrientation::normalizeToNose()
{
	nose.normalize();

	// Make sure head vector is perpendicular to nose vector,
	const D3Vector leftSide(head, nose);
	head.cross(nose, leftSide);
	head.normalize();		
}


void D3PosAndOrientation::pitch_up(float rotationStep)
{
	// We shall rotate the nose and head vectors in the axis of a vector perpendicular to both.
	nose.add(rotationStep, head);
	nose.normalize();
	head.add(-rotationStep, nose);
	head.normalize();
	
	normalizeToNose(); // Should not be needed, perhaps remove later.
}

void D3PosAndOrientation::yaw_right(float rotationStep)
{
	// We shall rotate the nose vector in the axis of the head vector
	const D3Vector right(nose, head);
	head.cross(right, nose);
	nose.add(rotationStep, right);
	nose.normalize();

	normalizeToNose(); // Should not be needed, perhaps remove later.
}


// rotationStep is in radians not degrees (but only for small angles, it can not rotate 3.14/2 radians in one go, call this several times instead.)
void D3PosAndOrientation::roll_right(float rotationStep)
{
	// We shall rotate the head vector in the axis of the nose vector
	// So first calculate the vector perpendicular to nose and head vectors
	D3Vector right(nose, head);
	head.add(rotationStep, right);
	head.normalize();

	normalizeToNose(); // Should not be needed, perhaps remove later.
}


void D3PosAndOrientation::forward(float distToMove)
{
	pos.add(distToMove, nose);
}


void D3PosAndOrientation::right(float distToMove)
{
	// We shall move in a direction perpendicular to nose and head vectors
	const D3Vector rightSide(nose, head);
	pos.add(distToMove, rightSide);
}

void D3PosAndOrientation::up(float distToMove)
{
	// Move in direction of head vector.
	pos.add(distToMove, head);
}


void D3PosAndOrientation::readSelf(WordReader *wr)
{
	pos.readSelf(wr);
	nose.readSelf(wr);
	head.readSelf(wr);
}

void D3PosAndOrientation::writeSelf(WordWriter *ww)
{
	pos.writeSelf(ww);
	nose.writeSelf(ww);
	head.writeSelf(ww);
}




// "m" must be an array of float with at least size 16 floats.
void D3PosAndOrientation::getInverseRotMatrix(float m[]) const
{
		float t[16];
		t[rm00]=m[rm00];		t[rm01]=m[rm10];		t[rm02]=m[rm20];		t[rm03]=m[rm30];
		t[rm10]=m[rm01];		t[rm11]=m[rm11];		t[rm12]=m[rm21];		t[rm13]=m[rm31];
		t[rm20]=m[rm02];		t[rm21]=m[rm12];		t[rm22]=m[rm22];		t[rm23]=m[rm32];
		t[rm30]=m[rm03];		t[rm31]=m[rm13];		t[rm32]=m[rm23];		t[rm33]=m[rm33];

		//memcpy(m, i, sizeof(i));
		for(int i=0;i<16;++i)
		{
			m[i]=t[i];
		}
}


// Translation matrix to be used when translating from this to that coordinate system of super (alias parent) room
void D3PosAndOrientation::getTransMatrixToSuper(const D3Vector &pos, float m[]) const
{
	m[rm00]=1;			m[rm01]=0;			m[rm02]=0;			m[rm03]=pos.x;
	m[rm10]=0;			m[rm11]=1;			m[rm12]=0;			m[rm13]=pos.y;
	m[rm20]=0;			m[rm21]=0;			m[rm22]=1;			m[rm23]=pos.z;
	m[rm30]=0;			m[rm31]=0;			m[rm32]=0;			m[rm33]=1;
}


// Rot matrix to be used when translating from this to that coordinate system of super (alias parent) room
// Same as getRotMatrix(1, 0, 0)
void D3PosAndOrientation::getRotMatrixToSuper(float m[]) const
{
	D3Vector rightSide(nose, head);

	m[rm00]=nose.x;      m[rm01]=nose.y;      m[rm02]=nose.z;      m[rm03]=0;
	m[rm10]=rightSide.x; m[rm11]=rightSide.y; m[rm12]=rightSide.z; m[rm13]=0;
	m[rm20]=-head.x;     m[rm21]=-head.y;     m[rm22]=-head.z;     m[rm23]=0;
	m[rm30]=0;           m[rm31]=0;           m[rm32]=0;           m[rm33]=1;

	getInverseRotMatrix(m);
}


// Translation matrix for transformations from super (AKA parent room) to local coordinate system
//
void D3PosAndOrientation::paoToMatrixToSuper(float m[]) const
{
	float m1[16];
	float m2[16];

	getRotMatrixToSuper(m1);
	getTransMatrixToSuper(pos, m2);

	MulMatrix4x4(m2,m1, m);
}


// Translation matrix for rotation transformations from super (AKA parent room) to local coordinate system
// Same as
// getRotMatrix(0, 0, 0);
//
void D3PosAndOrientation::getRotMatrixToLocal(float m[]) const
{
	D3Vector rightSide(nose, head);

	m[rm00]=nose.x;			m[rm01]=nose.y;			m[rm02]=nose.z;			m[rm03]=0;
	m[rm10]=rightSide.x;	m[rm11]=rightSide.y;	m[rm12]=rightSide.z;	m[rm13]=0;
	m[rm20]=-head.x;		m[rm21]=-head.y;		m[rm22]=-head.z;		m[rm23]=0;
	m[rm30]=0;				m[rm31]=0;				m[rm32]=0;				m[rm33]=1;
}


// Translation matrix for positional transformations from super (AKA parent room) to local coordinate system
// Same as
// getTransMatrix(pos, 0);
//
void D3PosAndOrientation::getTransMatrixToLocal(const D3Vector &pos, float m[]) const
{
	m[rm00]=1;			m[rm01]=0;			m[rm02]=0;			m[rm03]=-pos.x;
	m[rm10]=0;			m[rm11]=1;			m[rm12]=0;			m[rm13]=-pos.y;
	m[rm20]=0;			m[rm21]=0;			m[rm22]=1;			m[rm23]=-pos.z;
	m[rm30]=0;			m[rm31]=0;			m[rm32]=0;			m[rm33]=1;
}



// Translation matrix for transformations from super (AKA parent room) to local coordinate system
//
void D3PosAndOrientation::paoToMatrixToLocal(float m[]) const
{
	float m1[16];
	float m2[16];

	getRotMatrixToLocal(m1);
	getTransMatrixToLocal(pos, m2);

	MulMatrix4x4(m1,m2, m);
}




// Translate "pao" from local room coordinates to that of the super room (not recursively to global)
// From own (that of this object) coordinate system to that of super room
// This is the inverse of translatePaoToLocal
void D3PosAndOrientation::translatePaoToSuper(D3PosAndOrientation *pao) const
{
	float m[16];
	float r[16];

	paoToMatrixToSuper(m);

	getRotMatrixToSuper(r);

	pao->nose.MulMatrix4x4(r);

	pao->head.MulMatrix4x4(r);

	pao->pos.MulMatrix4x4(m);
}


// Translate "pao" from super room coordinates to that of the local room (not recursively from global)
// From super room coordinate system to that of own (that of this object)
// This is the inverse of translatePaoToSuper
void D3PosAndOrientation::translatePaoToLocal(D3PosAndOrientation *pao) const
{
	float m[16];
	float r[16];


	paoToMatrixToLocal(m);

	getRotMatrixToLocal(r);

	pao->nose.MulMatrix4x4(r);

	pao->head.MulMatrix4x4(r);

	pao->pos.MulMatrix4x4(m);
}



void D3PosAndOrientation::MulMatrix4x4(const float m1[], const float m2[], float m3[]) const
{

	m3[rm00] = m1[rm00]*m2[rm00] + m1[rm01]*m2[rm10] + m1[rm02]*m2[rm20] + m1[rm03]*m2[rm30];
	m3[rm01] = m1[rm00]*m2[rm01] + m1[rm01]*m2[rm11] + m1[rm02]*m2[rm21] + m1[rm03]*m2[rm31];
	m3[rm02] = m1[rm00]*m2[rm02] + m1[rm01]*m2[rm12] + m1[rm02]*m2[rm22] + m1[rm03]*m2[rm32];
	m3[rm03] = m1[rm00]*m2[rm03] + m1[rm01]*m2[rm13] + m1[rm02]*m2[rm23] + m1[rm03]*m2[rm33];

	m3[rm10] = m1[rm10]*m2[rm00] + m1[rm11]*m2[rm10] + m1[rm12]*m2[rm20] + m1[rm13]*m2[rm30];
	m3[rm11] = m1[rm10]*m2[rm01] + m1[rm11]*m2[rm11] + m1[rm12]*m2[rm21] + m1[rm13]*m2[rm31];
	m3[rm12] = m1[rm10]*m2[rm02] + m1[rm11]*m2[rm12] + m1[rm12]*m2[rm22] + m1[rm13]*m2[rm32];
	m3[rm13] = m1[rm10]*m2[rm03] + m1[rm11]*m2[rm13] + m1[rm12]*m2[rm23] + m1[rm13]*m2[rm33];

	m3[rm20] = m1[rm20]*m2[rm00] + m1[rm21]*m2[rm10] + m1[rm22]*m2[rm20] + m1[rm23]*m2[rm30];
	m3[rm21] = m1[rm20]*m2[rm01] + m1[rm21]*m2[rm11] + m1[rm22]*m2[rm21] + m1[rm23]*m2[rm31];
	m3[rm22] = m1[rm20]*m2[rm02] + m1[rm21]*m2[rm12] + m1[rm22]*m2[rm22] + m1[rm23]*m2[rm32];
	m3[rm23] = m1[rm20]*m2[rm03] + m1[rm21]*m2[rm13] + m1[rm22]*m2[rm23] + m1[rm23]*m2[rm33];

	m3[rm30] = m1[rm30]*m2[rm00] + m1[rm31]*m2[rm10] + m1[rm32]*m2[rm20] + m1[rm33]*m2[rm30];
	m3[rm31] = m1[rm30]*m2[rm01] + m1[rm31]*m2[rm11] + m1[rm32]*m2[rm21] + m1[rm33]*m2[rm31];
	m3[rm32] = m1[rm30]*m2[rm02] + m1[rm31]*m2[rm12] + m1[rm32]*m2[rm22] + m1[rm33]*m2[rm32];
	m3[rm33] = m1[rm30]*m2[rm03] + m1[rm31]*m2[rm13] + m1[rm32]*m2[rm23] + m1[rm33]*m2[rm33];

}


