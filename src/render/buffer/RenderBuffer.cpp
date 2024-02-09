#include "RenderBuffer.h"
#include "macros.h"

void RenderBuffer::_delRB()
{
	glDeleteFramebuffers(1, &rb_ID);
	rb_ID = 0;
}

RenderBuffer::RenderBuffer()
{
	glGenRenderbuffers(1, &rb_ID);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_W, SCREEN_H);
}

RenderBuffer::RenderBuffer(GLuint _type)
{
	glGenRenderbuffers(1, &rb_ID);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, _type, SCREEN_W, SCREEN_H);
}

RenderBuffer::~RenderBuffer()
{
	if (GetRenderBufferID() != 0)
		_delRB();
}

void RenderBuffer::BindRenderBuffer() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);

}

void RenderBuffer::UnbindRenderBuffer() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::Resize(float w, float h)
{
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
}

void RenderBuffer::Resize(const glm::vec2& size)
{
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
}

RenderBuffer::RenderBuffer(const RenderBuffer& rb)
{
	_resetRBID(rb.GetRenderBufferID());
}

RenderBuffer::RenderBuffer(RenderBuffer&& rb) noexcept
{
	_resetRBID(rb.GetRenderBufferID());
	rb.rb_ID = 0;
}

RenderBuffer& RenderBuffer::operator=(const RenderBuffer& rb)
{
	if (this == &rb)
		return *this;

	_resetRBID(rb.GetRenderBufferID());

	return *this;
}

RenderBuffer& RenderBuffer::operator=(RenderBuffer&& rb) noexcept
{
	if (this == &rb)
		return *this;

	_resetRBID(rb.GetRenderBufferID());
	rb.rb_ID = 0;

	return *this;
}
