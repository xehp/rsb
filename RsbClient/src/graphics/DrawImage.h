/*
DrawText.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#ifndef DRAW_IMAGE_H
#define DRAW_IMAGE_H

#ifndef ZERO_GL
#define ZERO_GL 0
#endif



int drawImageInit();
void drawImageQuit();
GLuint drawImageTextureFactory(int xOffset, int yOffset, int width, int height);

#endif
