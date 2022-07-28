#include "RenderBuffer.h"

RenderBuffer::RenderBuffer()
{
	glGenRenderbuffers(1, &rb_ID);
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_W, SCREEN_H);
}

RenderBuffer::~RenderBuffer()
{

}

void RenderBuffer::BindRenderBuffer() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, rb_ID);

}

void RenderBuffer::UnbindRenderBuffer() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
