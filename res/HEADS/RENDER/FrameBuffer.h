#pragma once
#include "Texture.h"
#include "RenderBuffer.h"
class FrameBuffer
{
private:
	GLuint fb_ID = 0, fb_tex_ID = 0;
public:
	Texture BufferTexture;
	RenderBuffer renderBuffer;

	FrameBuffer();
	~FrameBuffer();

	void BindFrameBuffer() const;
	void UnbindFrameBuffer() const;

	void BindFrameBufferTex() const;
	void UnbindFrameBufferTex() const;

	GLuint GetFrameBufferID() const { return fb_ID; }
	GLuint GetFrameBufferTextureID() const { return fb_tex_ID; }
};

