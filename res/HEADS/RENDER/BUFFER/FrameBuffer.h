#pragma once
#include "Texture.h"
#include "RenderBuffer.h"
#include <cstdarg>
#include <optional>

enum FBType
{
	/* nothing */	NONE_FB = -1,
	/*     HDR */	COMBINE_FB, EMIS_COL_FB, LIGHT_AO_FB = 1, POS_FB, MRSE_FB, NORMAL_FB, // MRSE: metalness roughness specular emission 
	/*     RGB */	ALBEDO_FB, MASK_FB, RAND_FB, ID_FB,
	/*   Float */	SINGLE_FB, SHADOW_FB,
					MAX_FB,
	/*   Depth */	DEPTH_FB = GL_DEPTH_ATTACHMENT - GL_COLOR_ATTACHMENT0
};

#define AVAIL_PASSES { EMIS_COL_FB, POS_FB, NORMAL_FB, ALBEDO_FB, MRSE_FB, RAND_FB, ID_FB, MASK_FB }

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
	mutable std::unordered_map<FBType, int> fb_type_list;      //  tex_type -> tex_index
	static TextureType PareseTexType(FBType _type);

public:
	std::optional<RenderBuffer> renderBuffer;

	FBType fb_type = NONE_FB;
	mutable std::vector<Texture> fb_tex_list;

	FrameBuffer();
	FrameBuffer(FBType type, GLuint attach=0);
	FrameBuffer(int count, ...);
	FrameBuffer(const std::vector<FBType>& _tars);
	~FrameBuffer();

	void BindFrameBuffer() const;
	void UnbindFrameBuffer() const;

	void Resize(const ImVec2& size, bool all = false);
	void Resize(float w, float h, bool all = false);
	ImVec2 GetSize() const { return { fb_w, fb_h }; }

	FBPixel ReadPix(GLuint x, GLuint y, FBType type);

	void BindFrameBufferTex(int count = 0, ...) const;
	void BindFrameBufferTex(const std::vector<FBType>& _tars) const;
	void BindFrameBufferTex(FBType tar, GLuint slot) const;
	void BindFrameBufferTexR(FBType tar, GLuint slot) const;
	void UnbindFrameBufferTexR(FBType tar, GLuint slot) const;
	void UnbindFrameBufferTex() const;

	void Del() const;

	GLuint GetFrameBufferID() const { return fb_ID; }
	const ImVec2&& GetFrameBufferSize() const { return ImVec2(fb_w, fb_h); }
	GLuint GetFBTextureID(FBType type) const { return fb_tex_list[fb_type_list[type]].GetTexID(); }
	Texture* GetFBTexturePtr(FBType type) const { return &fb_tex_list[fb_type_list[type]]; }
	size_t GetFBCount() const { return fb_tex_list.size(); }
	GLuint GetAttachmentLoc(FBType type) const { return fb_type_list[(FBType)type]; }
};

