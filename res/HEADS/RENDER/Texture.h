#pragma once
#include <iostream>
#include <string>
#include <GL/glew.h>

#include "support.h"

#include "Shaders.h"

enum TextureType
{
	NONE_TEXTURE,
	RGBA_TEXTURE,
	PNG_TEXTURE = 1,
	RGB_TEXTURE,
	JPG_TEXTURE = 2,
	SPIRIT_TEXURE,
	IBL_TEXTURE,
	HDR_TEXTURE = 4,
	IBL_CUBE_TEXTURE = 5,
	BUFFER_TEXTURE = 6,		// [0, 1]
	HDR_BUFFER_TEXTURE,		// [-inf, +inf]
	FLOAT_BUFFER_TEXTURE,
	RG_TEXTURE,
};

class Texture
{
private:

	static std::string root_dir;

private:

	int im_w = 0, im_h = 0, im_bpp = 0;
	std::string tex_path;
	GLuint tex_ID = 0;

public:

	TextureType tex_type = TextureType::NONE_TEXTURE;
	GLuint tex_slot_offset = 0;

	Texture(const std::string& texpath, TextureType tex_type, GLuint Tile_type); // using tex_type as its slot, but you can change it if you want
	Texture(GLuint Tile_type, int x, int y);									 // for FBO
	Texture(int _w, int _h, GLuint _layout, void* _ptr);						 // for generated texture
	Texture(int _w, int _h, GLuint _ID, TextureType _type, std::string _name);
	Texture();
	~Texture();

	void DelTexture() const;
	void Resize(const ImVec2& size);
	void Resize(float x, float y);
	void OffsetSlot(GLuint _offset) { tex_slot_offset += _offset; };

public:
	void Bind(GLuint slot = -1) const;
	void BindC(GLuint slot = -1, GLuint read_or_write = GL_READ_WRITE, GLuint _level = 0) const;
	void BindU(GLuint slot = -1) const;
	void UnbindC(GLuint slot = -1, GLuint read_or_write = GL_READ_WRITE, GLuint _level = 0) const;
	void Unbind() const;

public:
	inline int GetW()const { return im_w; }
	inline int GetH()const { return im_h; }
	inline int GetDPP()const { return im_bpp; }
	GLuint GetTexID() const { return tex_ID; }

public:
	using TexStorageInfo = std::tuple<GLuint, GLuint, GLuint, GLuint>; // internal_layout | layout | data_type | texture_type

	inline static TexStorageInfo ParseFormat(TextureType _type);
	template<GLuint Type>
	inline static void SetTexParam(GLuint _id, GLuint _fil_min, GLuint _fil_max, GLuint _warp_s = 0, GLuint _warp_t = 0, GLuint _lev_min = 0, GLuint _lev_max = 0, GLuint _warp_r = 0);

public: // for texture processing
	void GenIrradiaceConvFrom(GLuint _Tar_ID);
	void GenIrradiaceConvFrom(const Texture& _Tar_Tex);

	void GenCubeMapFrom(GLuint _Tar_ID, size_t res = 1024);
	void GenCubeMapFrom(const Texture& _Tar_Tex, size_t res = 1024);

private:
	void GenIrradianceConv(GLuint _tar_ID, size_t _tar_w, size_t _tar_h, TextureType _tar_type = IBL_TEXTURE);
	void GenCubeMap(GLuint _tar_ID, size_t _tar_res, TextureType _tar_type = IBL_CUBE_TEXTURE);
};


class TextureLib {

	using TextureRes = std::shared_ptr<Texture>;

public:
	enum NoiseType
	{
		NONE_NOISE, UNIFORM_NOISE, GAUSSIAN_NOISE, UNI_2D_NOISE
	};

private:
	static std::unordered_map<std::string, TextureRes> t_tex_list;

public:
	static TextureRes GetTexture(const std::string& _name);
	static GLuint GetTextureID(const std::string& _name);

public:
	static TextureRes Noise_2D_4x4();
	static TextureRes Noise_2D_16x16();

	static TextureRes IBL_LUT();

private:
	static void GenNoiseTexture(NoiseType _type, size_t _w, size_t _h);

};

