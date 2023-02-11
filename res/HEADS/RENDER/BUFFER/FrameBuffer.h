#pragma once
#include "Texture.h"
#include "RenderBuffer.h"
#include <cstdarg>
#include <optional>

enum FBType
{
	/* nothing */	NONE_FB = -1,
	/*     HDR */	COMBINE_FB, POS_FB, MRS_FB,
	/*     RGB */	NORMAL_FB, ALBEDO_FB, EMIS_COL_FB, RAND_FB, ID_FB,
	/*   Float */	ALPHA_FB, MASK_FB, EMIS_STH_FB, SHADOW_FB,
					MAX_FB,
	/*   Depth */	DEPTH_FB = GL_DEPTH_ATTACHMENT - GL_COLOR_ATTACHMENT0
};

#define AVAIL_PASSES 6, COMBINE_FB, POS_FB, RAND_FB, ID_FB, ALPHA_FB, MASK_FB

struct FBPixel
{
	float RGBA[4];
	int GetID() const { return RGBA[0]; }
};

class FrameBuffer
{
private:
	GLuint fb_ID = 0, fb_attach = 0;
	float fb_w = SCREEN_W, fb_h = SCREEN_H;
public:
	Texture BufferTexture;
	std::optional<RenderBuffer> renderBuffer;

	FBType fb_type = NONE_FB;
	mutable std::unordered_map<FBType, int> fb_type_list;      //  tex_type -> tex_index
	mutable std::vector<Texture> fb_tex_list;
	bool using_list = false;

	FrameBuffer();
	FrameBuffer(FBType type, GLuint attach=0);
	FrameBuffer(int count, ...);
	~FrameBuffer();

	void BindFrameBuffer() const;
	void UnbindFrameBuffer() const;

	void Resize(const ImVec2& size, bool all = false);
	void Resize(float w,float h, bool all = false);

	FBPixel ReadPix(GLuint x, GLuint y, FBType type);

	void BindFrameBufferTex(int count = 0, ...) const;
	void UnbindFrameBufferTex() const;

	void Del() const;

	GLuint GetFrameBufferID() const { return fb_ID; }
	const ImVec2&& GetFrameBufferSize() const { return ImVec2(fb_w, fb_h); }
	GLuint GetFBTextureID(FBType type) const { return fb_tex_list[fb_type_list[type]].GetTexID(); }
	size_t GetFBCount() const { return fb_tex_list.size(); }
	GLuint GetAttachmentLoc(FBType type) const { return fb_type_list[(FBType)type]; }
};

