#include "RenderBuffer.h"
#include "macros.h"

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
	//glDeleteRenderbuffers(1, &rb_ID);
	//DEBUG("RB dele")
}

void RenderBuffer::BindRenderBuffer() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);

}

void RenderBuffer::UnbindRenderBuffer() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::Del() const
{
	glDeleteRenderbuffers(1, &rb_ID);
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
