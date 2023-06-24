#include "FrameBuffer.h"

TextureType FrameBuffer::PareseTexType(FBType _type)
{
	TextureType textype = NONE_TEXTURE;
	if (COMBINE_FB <= _type && _type <= NORMAL_FB)
		textype = HDR_BUFFER_TEXTURE;
	else if (ALBEDO_FB <= _type && _type <= ID_FB)
		textype = BUFFER_TEXTURE;
	else if (SINGLE_FB <= _type && _type <= SHADOW_FB)
		textype = FLOAT_BUFFER_TEXTURE;
	else if (_type == -32) {}

	return textype;
}

FrameBuffer::FrameBuffer()
{
}


FrameBuffer::FrameBuffer(FBType type/*=NONE_FB*/, GLuint attach)
	:renderBuffer(RenderBuffer(GL_DEPTH24_STENCIL8)), fb_type(type)
{
	fb_type_list[(FBType)type] = 0;
	TextureType textype = FrameBuffer::PareseTexType(type);
	fb_tex_list.emplace_back("", textype, GL_NEAREST);
	fb_tex_list[0].OffsetSlot(type);

	//IDTexture = Texture("", BUFFER_TEXTURE, GL_NEAREST);
	//IDTexture.SlotAdd(1);
	//BufferTexture.Bind(BufferTexture.Tex_type);

	glGenFramebuffers(1, &fb_ID);//GLDEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + fb_type, GL_TEXTURE_2D, fb_tex_list[0].GetTexID(), 0);
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
	:renderBuffer(RenderBuffer(GL_DEPTH24_STENCIL8))
{
	glGenFramebuffers(1, &fb_ID);//GLDEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
	GLDEBUG
	std::vector<GLenum> attachments(count);
	va_list arg_ptr;
	va_start(arg_ptr, count);
	LOOP(count) {
		int type_inp = va_arg(arg_ptr, int);
		TextureType textype = FrameBuffer::PareseTexType((FBType)type_inp);

		fb_type_list[(FBType)type_inp] = i;
		fb_tex_list.emplace_back("", textype, GL_NEAREST);
		fb_tex_list[i].OffsetSlot(type_inp);
		
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, fb_tex_list[i].GetTexID(), 0);

	}
	va_end(arg_ptr);
	GLDEBUG
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->GetRenderBufferID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUG("framebuffer error")
	}
	else
	{
		std::cout << "framebuffer is complete\n";
	}
	glDrawBuffers(count, attachments.data());
	UnbindFrameBuffer();
}

FrameBuffer::FrameBuffer(const std::vector<FBType>& _tars)
	:renderBuffer(RenderBuffer(GL_DEPTH24_STENCIL8))
{
	glGenFramebuffers(1, &fb_ID);//GLDEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
	GLDEBUG
	std::vector<GLenum> attachments(_tars.size());

	int i = 0;
	for (auto type_inp : _tars) {
		TextureType textype = FrameBuffer::PareseTexType(type_inp);

		fb_type_list[(FBType)type_inp] = i;
		fb_tex_list.emplace_back("", textype, GL_NEAREST);
		fb_tex_list[i].OffsetSlot(type_inp);

		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, fb_tex_list[i].GetTexID(), 0);

		i++;
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->GetRenderBufferID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUG("framebuffer error")
	}
	else
	{
		std::cout << "framebuffer is complete\n";
	}
	glDrawBuffers(attachments.size(), attachments.data());
	UnbindFrameBuffer();

}

FrameBuffer::FrameBuffer(const Texture& _depth)
{
	auto [_1, _2, _3, gl_type] = Texture::ParseFormat(_depth.tex_type);

	glGenFramebuffers(1, &fb_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_ID);
	switch (gl_type)
	{
	case GL_TEXTURE_2D:
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, gl_type, _depth.GetTexID(), 0);
		break;
	case GL_TEXTURE_CUBE_MAP:
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth.GetTexID(), 0);
		break;
	}
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	UnbindFrameBuffer();
}

FrameBuffer::~FrameBuffer()
{
	//DEBUG("FB dele")
}

void FrameBuffer::BindFrameBuffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb_ID);
}

void FrameBuffer::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::LinkTexture(const Texture& _tex)
{
	auto [_1, _2, _3, gl_type] = Texture::ParseFormat(_tex.tex_type);

	BindFrameBuffer();

	switch (gl_type)
	{
	case GL_TEXTURE_2D:
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, gl_type, _tex.GetTexID(), 0);
		break;
	case GL_TEXTURE_CUBE_MAP:
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _tex.GetTexID(), 0);
		break;
	}

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	UnbindFrameBuffer();
}

void FrameBuffer::Resize(const ImVec2& size, bool all)
{
	Resize(size[0], size[1], all);
}

void FrameBuffer::Resize(float w, float h, bool all)
{
	fb_w = w;
	fb_h = h;
	renderBuffer->Resize(w, h);
	for (auto& tex : fb_tex_list)
		tex.Resize(w, h);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
}

//https://docs.gl/gl3/glReadPixels
FBPixel FrameBuffer::ReadPix(GLuint x, GLuint y, FBType type)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_ID);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + fb_type_list[type]);

	FBPixel Pixel;
	glReadPixels(x, fb_h - y + 20, 1, 1, GL_RGBA, GL_FLOAT, &Pixel);

	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	Pixel.RGBA[0] = glm::round(Pixel.RGBA[0] * 256);
	return Pixel;
}

void FrameBuffer::BindFrameBufferTex(int count, ...) const
{
	if (count == 0) {
		for(auto& pair : fb_type_list)
			fb_tex_list[pair.second].Bind(BUFFER_TEXTURE + pair.first);
		return;
	}

	va_list arg_ptr;
	va_start(arg_ptr, count);
	LOOP(count) {
		int type = va_arg(arg_ptr, int);
		fb_tex_list[fb_type_list[(FBType)type]].Bind(BUFFER_TEXTURE + type);
	}
	va_end(arg_ptr);
}

void FrameBuffer::BindFrameBufferTex(const std::vector<FBType>& _tars) const
{
	for (FBType tar : _tars) {
		fb_tex_list[fb_type_list[tar]].Bind(BUFFER_TEXTURE + tar);
	}
}

void FrameBuffer::BindFrameBufferTex(FBType tar, GLuint slot) const
{
	fb_tex_list[fb_type_list[tar]].Bind(slot);
}

void FrameBuffer::BindFrameBufferTexR(FBType tar, GLuint slot) const
{
	fb_tex_list[fb_type_list[tar]].BindC(slot, GL_READ_WRITE);
}

void FrameBuffer::UnbindFrameBufferTexR(FBType tar, GLuint slot) const
{
	fb_tex_list[fb_type_list[tar]].UnbindC(slot);
	fb_tex_list[fb_type_list[tar]].Unbind();
}

void FrameBuffer::UnbindFrameBufferTex() const
{
	for (auto& tar : fb_tex_list) {
		tar.Unbind();
	}
}

void FrameBuffer::Del() const
{
	renderBuffer->Del();
	for (auto& tex : fb_tex_list)
		tex.DelTexture();
}