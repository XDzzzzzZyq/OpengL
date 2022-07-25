#pragma once
#include "Texture.h"

class RenderBuffer
{
private:
	GLuint rb_ID = 0;
public:


	RenderBuffer();
	~RenderBuffer();

	void BindRenderBuffer() const;
	void UnbindRenderBuffer() const;

	GLuint GetRenderBufferID() const { return rb_ID; }
};

