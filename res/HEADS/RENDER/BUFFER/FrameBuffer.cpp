#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
}


FrameBuffer::FrameBuffer(FBType type/*=NONE_FB*/, GLuint attach)
	:fb_type(type), using_list(false)
{
	
	renderBuffer = RenderBuffer();

	BufferTexture = Texture("", BUFFER_TEXTURE, GL_NEAREST);
	IDTexture = Texture("", BUFFER_TEXTURE, GL_NEAREST);
	IDTexture.SlotAdd(1);
	//BufferTexture.Bind(BufferTexture.Tex_type);

	glGenFramebuffers(1, &fb_ID);//GLDEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + fb_type, GL_TEXTURE_2D, BufferTexture.GetTexID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + fb_type + 1, GL_TEXTURE_2D,IDTexture.GetTexID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->GetRenderBufferID());
	
	//glDrawBuffer(fb_type);GLDEBUG
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUG("framebuffer error")
	}
	else
	{
		std::cout << "framebuffer is complete\n";
	}
	GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	
	//UnbindFrameBuffer();
}

FrameBuffer::FrameBuffer(int count, ...)
	:using_list(true)
{
	renderBuffer = RenderBuffer();

	glGenFramebuffers(1, &fb_ID);//GLDEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);

	GLenum* attachments = new GLenum[count];

	va_list arg_ptr;
	va_start(arg_ptr, count);
	LOOP(count) {
		int type = va_arg(arg_ptr, int);
		fb_type_list[(FBType)type] = i;
		fb_tex_list.push_back(Texture("", BUFFER_TEXTURE, GL_NEAREST));
		fb_tex_list[i].SlotAdd(type);

		attachments[i] = GL_COLOR_ATTACHMENT0 + type;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, fb_tex_list[i].GetTexID(), 0);

	}
	va_end(arg_ptr);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->GetRenderBufferID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUG("framebuffer error")
	}
	else
	{
		std::cout << "framebuffer is complete\n";
	}
	glDrawBuffers(count, attachments);
	UnbindFrameBuffer();
	delete[] attachments;
}

FrameBuffer::~FrameBuffer()
{
	//DEBUG("FB dele")
}

void FrameBuffer::BindFrameBuffer() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
}

void FrameBuffer::UnbindFrameBuffer() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(const ImVec2& size, bool all)
{
	fb_w = size[0];
	fb_h = size[1];
	renderBuffer->Resize(size);

	if (using_list) {
		for (auto& tex : fb_tex_list) 
			tex.Resize(size);
	}
	else {
		IDTexture.Resize(size);
		BufferTexture.Resize(size);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
}

void FrameBuffer::Resize(float w, float h, bool all)
{
	fb_w = w;
	fb_h = h;
	renderBuffer->Resize(w, h);
	if (using_list) {
		for (auto& tex : fb_tex_list)
			tex.Resize(w, h);
	}
	else {
		IDTexture.Resize(w, h);
		BufferTexture.Resize(w, h);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
}

//https://docs.gl/gl3/glReadPixels
FBPixel FrameBuffer::ReadPix(GLuint x, GLuint y, FBType type)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_ID);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + type);

	FBPixel Pixel;
	glReadPixels(x, fb_h - y + 40, 1, 1, GL_RGBA, GL_FLOAT, &Pixel);

	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	Pixel.RGBA[0] = glm::round(Pixel.RGBA[0] * 256);
	return Pixel;
}

void FrameBuffer::BindFrameBufferTex() const
{
	BufferTexture.Bind();
	IDTexture.Bind();
}

void FrameBuffer::BindFrameBufferTex(int count, ...) const
{
	va_list arg_ptr;
	va_start(arg_ptr, count);
	LOOP(count) {
		int type = va_arg(arg_ptr, int);
		fb_tex_list[fb_type_list[(FBType)type]].Bind(BUFFER_TEXTURE + type);
	}
	va_end(arg_ptr);
}

void FrameBuffer::UnbindFrameBufferTex() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::Del() const
{
	renderBuffer->Del();
	BufferTexture.DelTexture();
	IDTexture.DelTexture();

	for (auto& tex : fb_tex_list)
		tex.DelTexture();
}