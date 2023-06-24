#pragma once
#include "Texture.h"

class RenderBuffer
{
private:
	GLuint rb_ID = 0;
public:


	RenderBuffer();
	RenderBuffer(GLuint _type);
	~RenderBuffer();

	void BindRenderBuffer() const;
	void UnbindRenderBuffer() const;

	void Del() const;

	void Resize(float w, float h);
	void Resize(const ImVec2& size);

	GLuint GetRenderBufferID() const { return rb_ID; }
};

