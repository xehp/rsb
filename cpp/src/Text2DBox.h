/*
Text2DBox.h

www.eit.se/rsb
Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
License: www.eit.se/rsb/license

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/

#include "MirrorContainer.h"

class Text2DBox : public MirrorContainer
{
public:
	Text2DBox(float x, float y, float width, float height);

	~Text2DBox();

	virtual void renderAll();

	void setText(const char *str);
	void setButtonCommand(const char *str);
	void clearText();
	void setImage(int xOffset, int yOffset, int width, int height);


	GLuint vbo_vertices;
	GLuint ibo_elements;
	GLuint vbo_texcoords;
	GLuint texture_id;

	float x;
	float y;
	float width;
	float height;
	//char *textString;
	char *buttonCommand;

	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
private:
	Text2DBox(const Text2DBox&);
	Text2DBox& operator=(const Text2DBox&);

};
