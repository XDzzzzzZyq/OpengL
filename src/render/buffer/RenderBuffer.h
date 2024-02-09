#pragma once
#include "Texture.h"

class RenderBuffer
{
private:
	GLuint rb_ID = 0;

	void _delRB();
	void _resetRBID(GLuint _ID) { if (rb_ID > 0 && rb_ID != _ID)_delRB(); rb_ID = _ID; }
public:


	RenderBuffer();
	RenderBuffer(GLuint _type);
	~RenderBuffer();


	RenderBuffer(const RenderBuffer& rb);
	RenderBuffer(RenderBuffer&& rb) noexcept;

	RenderBuffer& operator=(const RenderBuffer& rb);
	RenderBuffer& operator=(RenderBuffer&& rb) noexcept;

public:

	void BindRenderBuffer() const;
	void UnbindRenderBuffer() const;

	void Resize(float w, float h);
	void Resize(const glm::vec2& size);

	GLuint GetRenderBufferID() const { return rb_ID; }
};
