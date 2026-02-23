#pragma once
#include "Texture.h"

/**
 * @brief OpenGL renderbuffer object wrapper.
 *
 * RenderBuffer encapsulates a GL renderbuffer (glGenRenderbuffers) used as
 * a depth/stencil attachment in FrameBuffer. Move semantics transfer the
 * renderbuffer ownership; copy assigns the same renderbuffer ID (shallow).
 *
 * @note Deep copy of a renderbuffer requires framebuffer blitting and is not
 *       implemented. Prefer moving renderbuffers rather than copying them.
 * @note GPU Resource Ownership: Owns rb_ID, released via glDeleteRenderbuffers
 *       in destructor.
 */
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

	void Resize(GLuint w, GLuint h);
	void Resize(const glm::vec2& size);

	GLuint GetRenderBufferID() const { return rb_ID; }
};
