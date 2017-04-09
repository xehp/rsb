/*
Text2DBox.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include "MirrorContainer.h"

class DrawText;

class Text2DBox : public MirrorContainer
{
public:


	/**
	 * x and y are the position of the center of the box on the screen,
	 * (-1,-1) is lower left and (1,1) is upper right.
	 * giving (0,0) would put the box in the center of the screen
	 * (with screen we actually mean the SDL mainWindow).
	 */
	Text2DBox(DrawText *drawText, float centerX, float centerY, float width, float height);
	virtual ~Text2DBox();
	virtual void renderAll();
	virtual void linkSelf(MirrorBase *parentPtr);
	void setText(const char *str);
	void setTextW(const wchar_t *str);
	void setButtonCommand(const char *str);
	void clearText();
	void setImage(int xOffset, int yOffset, int width, int height);

	DrawText *drawText;
	GLuint vbo_vertices;
	GLuint ibo_elements;
	GLuint vbo_texcoords;
	GLuint texture_id;

	//float xPos;
	float xCenterPos;
	float yCenterPos;
	//float yPos;
	float width;
	float height;
	wchar_t *debugTextString;
	char *buttonCommand;
	unsigned int maxNumberOfCharactersInBox;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	Text2DBox(const Text2DBox&);
	Text2DBox& operator=(const Text2DBox&);

};
