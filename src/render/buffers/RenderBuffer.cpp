#include "RenderBuffer.h"
#include "macros.h"

void RenderBuffer::_delRB()
{
	glDeleteRenderbuffers(1, &rb_ID);
	rb_ID = 0;
}

void RenderBuffer::_deepCopyFrom(const RenderBuffer& rb)
{
	rb_format = rb.rb_format;
	rb_w      = rb.rb_w;
	rb_h      = rb.rb_h;

	if (rb.rb_ID == 0)
	{
		rb_ID = 0;
		return;
	}

	glGenRenderbuffers(1, &rb_ID);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, rb_format, (GLsizei)rb_w, (GLsizei)rb_h);
}

RenderBuffer::RenderBuffer()
	: rb_format(GL_DEPTH24_STENCIL8), rb_w(SCREEN_W), rb_h(SCREEN_H)
{
	glGenRenderbuffers(1, &rb_ID);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, rb_format, SCREEN_W, SCREEN_H);
}

RenderBuffer::RenderBuffer(GLuint _type)
	: rb_format(_type), rb_w(SCREEN_W), rb_h(SCREEN_H)
{
	glGenRenderbuffers(1, &rb_ID);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, rb_format, SCREEN_W, SCREEN_H);
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

void RenderBuffer::Resize(GLuint w, GLuint h)
{
	rb_w = w;
	rb_h = h;
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, rb_format, (GLsizei)w, (GLsizei)h);
}

void RenderBuffer::Resize(const glm::vec2& size)
{
	Resize((GLuint)size.x, (GLuint)size.y);
}

RenderBuffer::RenderBuffer(const RenderBuffer& rb)
{
	_deepCopyFrom(rb);
}

RenderBuffer::RenderBuffer(RenderBuffer&& rb) noexcept
{
	rb_ID     = rb.rb_ID;
	rb_format = rb.rb_format;
	rb_w      = rb.rb_w;
	rb_h      = rb.rb_h;
	rb.rb_ID  = 0;
}

RenderBuffer& RenderBuffer::operator=(const RenderBuffer& rb)
{
	if (this == &rb)
		return *this;

	if (rb_ID != 0)
		_delRB();

	_deepCopyFrom(rb);
	return *this;
}

RenderBuffer& RenderBuffer::operator=(RenderBuffer&& rb) noexcept
{
	if (this == &rb)
		return *this;

	if (rb_ID != 0)
		_delRB();

	rb_ID     = rb.rb_ID;
	rb_format = rb.rb_format;
	rb_w      = rb.rb_w;
	rb_h      = rb.rb_h;
	rb.rb_ID  = 0;
	return *this;
}
