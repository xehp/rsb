/*
D3Matrix4x4.cpp


Copyright (C) 2015 Henrik Björkman www.eit.se/hb

License: You may use, copy, modify and redistribute this code as long as you keep the History below and add state there if you did any changes.



www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license



History:
2015-03-09 Created by Henrik Bjorkman (www.eit.se/hb)

*/


//#include <SDL_opengl.h>

#include "D3Vector.h"
#include "D3PosAndOrientation.h"
#include "D3Matrix4x4.h"



// Default constructor
D3Matrix4x4::D3Matrix4x4()
{
	for (int i=0; i<16; i++)
	{
		matrix[i]=0;
	}
}




// Copy constructor
D3Matrix4x4::D3Matrix4x4(const D3Matrix4x4 &d)
{
	for (int i=0; i<16; i++)
	{
		matrix[i]=d.matrix[i];
	}
}


// See D3PosAndOrientation.java
D3Matrix4x4::D3Matrix4x4(const D3PosAndOrientation &pao)
{
	D3Vector forwardLook(pao.nose);
	D3Vector up(pao.head);

	D3Vector rightSide(pao.nose, pao.head); // a vector pointing to objects right side


	matrix[cm00]=rightSide.x;		matrix[cm10]=rightSide.y;		matrix[cm20]=rightSide.z;		matrix[cm30]=-rightSide.dot(pao.pos);
	matrix[cm01]=up.x;				matrix[cm11]=up.y;				matrix[cm21]=up.z;				matrix[cm31]=-up.dot(pao.pos);
	matrix[cm02]=-forwardLook.x;	matrix[cm12]=-forwardLook.y;	matrix[cm22]=-forwardLook.z;	matrix[cm32]=forwardLook.dot(pao.pos);			
	matrix[cm03]=0;					matrix[cm13]=0;					matrix[cm23]=0;					matrix[cm33]=1;
}





