#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{

	renderBuffer = RenderBuffer();

	BufferTexture = Texture("", BUFFER_TEXTURE, GL_NEAREST);
	BufferTexture.Bind(BufferTexture.Tex_type);
	BufferTexture.Tex_slot = BufferTexture.Tex_type;

	glGenFramebuffers(1, &fb_ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BufferTexture.GetTexID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GetRenderBufferID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUG("framebuffer error")
	}
	else
	{
		std::cout << "framebuffer is complete\n";
	}
}

FrameBuffer::~FrameBuffer()
{

}

void FrameBuffer::BindFrameBuffer() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
}

void FrameBuffer::UnbindFrameBuffer() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::BindFrameBufferTex() const
{
	BufferTexture.Bind(BUFFER_TEXTURE);
}

void FrameBuffer::UnbindFrameBufferTex() const
{
	BufferTexture.Unbind();
}
