#pragma once

#include "RenderBuffer.h"
#include "macros.h"

#include <cstdarg>
#include <optional>
#include <unordered_map>
#include <vector>


enum FBType
{
	/* nothing */	NONE_FB = -1,
	/*     HDR */	COMBINE_FB = 0, 
					EMIS_COL_FB, LIGHT_AO_FB = 1, DIR_DIFF_FB = 1, 
					POS_FB,	     POS_B_FB = 2,	  DIR_SPEC_FB = 2,
					MRSE_FB,	 OPT_FLW_FB = 3,  IND_DIFF_FB = 3,			// MRSE: metalness roughness specular emission 
					NORMAL_FB,					  IND_SPEC_FB = 4,
												  DIR_EMIS_FB = 5,
	/*     RGB */	ALBEDO_FB, 
					MASK_FB, 
					RAND_FB, 
					ID_FB,
	/*   Float */	SINGLE_FB, 
					SHADOW_FB,
					MAX_FB,
	/*   Depth */	DEPTH_FB = GL_DEPTH_ATTACHMENT - GL_COLOR_ATTACHMENT0
};

#define AVAIL_PASSES { EMIS_COL_FB, POS_FB, NORMAL_FB, ALBEDO_FB, MRSE_FB, RAND_FB, ID_FB, MASK_FB }
#define RESULT_PASSES { COMBINE_FB, DIR_DIFF_FB, DIR_SPEC_FB, IND_DIFF_FB, IND_SPEC_FB, DIR_EMIS_FB }

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

public:

	FrameBuffer();
	FrameBuffer(FBType type, GLuint attach=0);			// for single framebuffer
	FrameBuffer(int count, ...);						// for compile-time framebuffer list
	FrameBuffer(const std::vector<FBType>& _tars);		// for run-time framebuffer list
	FrameBuffer(const Texture& _depth);					// for depth framebuffer
	~FrameBuffer();

	void BindFrameBuffer() const;
	static void UnbindFrameBuffer();

	void LinkTexture(const Texture& _tex);

public:

	void Resize(const glm::vec2& size, bool all = false);
	void Resize(float w, float h, bool all = false);
	glm::vec2 GetSize() const { return { fb_w, fb_h }; }

	FBPixel ReadPix(GLuint x, GLuint y, FBType type);

public:

	void BindFrameBufferTex(int count = 0, ...) const;
	void BindFrameBufferTex(const std::vector<FBType>& _tars) const;
	void BindFrameBufferTex(FBType tar, GLuint slot) const;
	void BindFrameBufferTexR(FBType tar, GLuint slot) const;
	void UnbindFrameBufferTexR(FBType tar, GLuint slot) const;
	void UnbindFrameBufferTex() const;

public:

	void Del() const;

public:

	GLuint GetFrameBufferID() const { return fb_ID; }
	const glm::vec2&& GetFrameBufferSize() const { return { fb_w, fb_h }; }
	GLuint GetFBTextureID(FBType type) const { return fb_tex_list[fb_type_list[type]].GetTexID(); }
	Texture* GetFBTexturePtr(FBType type) const { return &fb_tex_list[fb_type_list[type]]; }
	size_t GetFBCount() const { return fb_tex_list.size(); }
	GLuint GetAttachmentLoc(FBType type) const { return fb_type_list[(FBType)type]; }
};

