#include "FrameBuffer.h"
#include "macros.h"

TextureType FrameBuffer::PareseTexType(FBType _type)
{
	TextureType textype = NONE_TEXTURE;
	if (ISIN(_type, 0, 1, 2, 3, 4, 5))
		textype = HDR_BUFFER_TEXTURE;
	else if (ISIN(_type, ALBEDO_FB, MASK_FB, RAND_FB, ID_FB))
		textype = BUFFER_TEXTURE;
	else if (ISIN(_type, SINGLE_FB, SHADOW_FB))
		textype = FLOAT_BUFFER_TEXTURE;
	else if (_type == -32) {}

	return textype;
}

void FrameBuffer::_cpyInfo(const FrameBuffer& fb)
{
	fb_ID = fb.GetFrameBufferID(); fb_attach = fb.fb_attach; fb_w = fb.fb_w; fb_h = fb.fb_h;
}

void FrameBuffer::_delFB()
{
	glDeleteFramebuffers(1, &fb_ID);
	fb_ID = 0;
}

FrameBuffer::FrameBuffer()
{}


FrameBuffer::FrameBuffer(FBType type/*=NONE_FB*/, GLuint attach)
	:renderBuffer(RenderBuffer(GL_DEPTH24_STENCIL8)), fb_w(SCREEN_W), fb_h(SCREEN_H)
{
	fb_type_list[(FBType)type] = 0;
	TextureType textype = FrameBuffer::PareseTexType(type);
	fb_tex_list.emplace_back("", textype, GL_LINEAR);
	fb_tex_list[0].OffsetSlot(type);

	//IDTexture = Texture("", BUFFER_TEXTURE, GL_NEAREST);
	//IDTexture.SlotAdd(1);
	//BufferTexture.Bind(BufferTexture.Tex_type);

	glGenFramebuffers(1, &fb_ID);//GLDEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + type, GL_TEXTURE_2D, fb_tex_list[0].GetTexID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->GetRenderBufferID());
	
	//glDrawBuffer(fb_type);GLDEBUG
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer error\n";
		_delFB();
	}
	else
	{
		DEBUG("framebuffer is complete\n");
	}
	GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	//UnbindFrameBuffer();
}

FrameBuffer::FrameBuffer(int count, ...)
	:renderBuffer(RenderBuffer(GL_DEPTH24_STENCIL8)), fb_w(SCREEN_W), fb_h(SCREEN_H)
{
	glGenFramebuffers(1, &fb_ID);//GLDEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
	GLDEBUG;
	std::vector<GLenum> attachments(count);
	va_list arg_ptr;
	va_start(arg_ptr, count);
	LOOP(count) {
		int type_inp = va_arg(arg_ptr, int);
		TextureType textype = FrameBuffer::PareseTexType((FBType)type_inp);

		fb_type_list[(FBType)type_inp] = i;
		fb_tex_list.emplace_back("", textype, GL_LINEAR);
		fb_tex_list[i].OffsetSlot(type_inp);
		
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, fb_tex_list[i].GetTexID(), 0);

	}
	va_end(arg_ptr);
	GLDEBUG;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->GetRenderBufferID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer error\n";
		_delFB();
	}
	else
	{
		DEBUG("framebuffer is complete");
	}
	glDrawBuffers(count, attachments.data());
	UnbindFrameBuffer();
}

FrameBuffer::FrameBuffer(const std::vector<FBType>& _tars)
	:renderBuffer(RenderBuffer(GL_DEPTH24_STENCIL8)), fb_w(SCREEN_W), fb_h(SCREEN_H)
{
	glGenFramebuffers(1, &fb_ID);//GLDEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
	
	std::vector<GLenum> attachments(_tars.size());
	fb_tex_list.reserve(_tars.size());

	for (int i = 0; auto type_inp : _tars) {
		TextureType textype = FrameBuffer::PareseTexType(type_inp);

		fb_type_list[(FBType)type_inp] = i;

		fb_tex_list.emplace_back("", textype, GL_LINEAR);
		fb_tex_list[i].OffsetSlot(type_inp);

		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, fb_tex_list[i].GetTexID(), 0);

		i++;
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->GetRenderBufferID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer error\n";
		_delFB();
	}
	else
	{
		DEBUG("framebuffer is complete");
	}
	glDrawBuffers(attachments.size(), attachments.data());
	UnbindFrameBuffer();

}

FrameBuffer::FrameBuffer(Texture&& _depth)
	:fb_w(SCREEN_W), fb_h(SCREEN_H)
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

	fb_tex_list.emplace_back(std::move(_depth));
}

FrameBuffer::FrameBuffer(const FrameBuffer& fb)
{
	_resetFBID(fb.GetFrameBufferID());
	_cpyInfo(fb);
	fb_type_list = fb.fb_type_list;
	fb_tex_list = fb.fb_tex_list;

	if (fb.renderBuffer)
		renderBuffer = fb.renderBuffer;
}

FrameBuffer::FrameBuffer(FrameBuffer&& fb) noexcept
{
	_cpyInfo(fb);
	fb_type_list = std::move(fb.fb_type_list);
	fb_tex_list = std::move(fb.fb_tex_list);

	if (fb.renderBuffer)
		renderBuffer = std::move(fb.renderBuffer);

	fb.fb_ID = 0;
}

FrameBuffer& FrameBuffer::operator=(const FrameBuffer& fb)
{
	if (this == &fb)
		return *this;

	_resetFBID(fb.GetFrameBufferID());
	_cpyInfo(fb);

	fb_type_list = fb.fb_type_list;
	fb_tex_list = fb.fb_tex_list;

	if (fb.renderBuffer)
		renderBuffer = fb.renderBuffer;

	return *this;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& fb) noexcept
{
	if (this == &fb)
		return *this;

	_cpyInfo(fb);

	fb_type_list = std::move(fb.fb_type_list);
	fb_tex_list = std::move(fb.fb_tex_list);

	if (fb.renderBuffer)
		renderBuffer = std::move(fb.renderBuffer);

	fb.fb_ID = 0;

	return *this;
}

FrameBuffer::~FrameBuffer()
{
	if (GetFrameBufferID() != 0)
		_delFB();
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
	auto [_1, _data, _3, gl_type] = Texture::ParseFormat(_tex.tex_type);

	auto attachment = _data == GL_DEPTH_COMPONENT ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0;

	BindFrameBuffer();

	switch (gl_type)
	{
	case GL_TEXTURE_2D:
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, gl_type, _tex.GetTexID(), 0);
		break;
	case GL_TEXTURE_CUBE_MAP:
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, _tex.GetTexID(), 0);
		break;
	}

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	UnbindFrameBuffer();
}

void FrameBuffer::AppendTexture(const Texture& _tex, FBType _type)
{
	std::vector<GLenum> attachments(fb_tex_list.size() + 1);
	auto [_1, _2, _3, gl_type] = Texture::ParseFormat(_tex.tex_type);

	LOOP(attachments.size())
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;

	fb_type_list[(FBType)_type] = fb_tex_list.size();
	fb_tex_list.emplace_back(_tex);
	fb_tex_list[fb_tex_list.size() - 1].OffsetSlot(_type);

	switch (gl_type)
	{
	case GL_TEXTURE_2D:
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[attachments.size()-1], gl_type, _tex.GetTexID(), 0);
		break;
	case GL_TEXTURE_CUBE_MAP:
		glFramebufferTexture(GL_FRAMEBUFFER, attachments[attachments.size() - 1], _tex.GetTexID(), 0);
		break;
	}

	glDrawBuffers(attachments.size(), attachments.data());
}

void FrameBuffer::Resize(const glm::vec2& size, bool all)
{
	Resize(size[0], size[1], all);
}

void FrameBuffer::Resize(GLuint w, GLuint h, bool all)
{
	fb_w = w;
	fb_h = h;
	renderBuffer->Resize(w, h);
	for (auto& tex : fb_tex_list)
		tex.Resize(w, h);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_ID);
}

#include "glm/glm.hpp"
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
