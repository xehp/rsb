/*
 * CubeProgram.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 24, 2015
 *      Author: henrik
 */

#ifndef CUBEPROGRAM_H_
#define CUBEPROGRAM_H_

extern GLuint cube3dProgramId;
extern GLint cube3dUniformMytexture;
extern GLint cube3dCoord3dAttribute;
extern GLint cube3dTexcoord_attribute;
extern GLint cube3dMatrixID;
extern GLuint cube3dTextureId;

extern int cube3dtextureWidth;
extern int cube3dtextureHeight;


int cube3dProgramInitGl();
void cube3dCleanup();

#endif /* CUBEPROGRAM_H_ */
