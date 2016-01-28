/*
 * HybridLib.h
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 *  Created on: Apr 30, 2015
 *      Author: henrik
 */

#ifndef HYBRIDLIB_H_
#define HYBRIDLIB_H_




extern int ourAvatarId;
//extern bool mouseIsCaptured;


void mainDebug(const char *msg, const char* str);
int mainSend(const char *str);

void mouseCapture();
void mouseRelease();
bool mouseIsPointerLock();



#endif /* HYBRIDLIB_H_ */
