#pragma once
#include <iostream>
#include <string>
#include <GL/glew.h>

#include "glm/glm.hpp"

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
	LIGHTING_CACHE = 30,
	DEPTH_CUBE_TEXTURE = 31,
	DEPTH_TEXTURE
};

/*
 * How to add a texture:
 * 1. Assign a (GLuint) slot for the texture (slot 0-14 are currently in use)
 * 2. Create a function that allocates a TextureRes in TextureLib
 * 3. Call that function and bind the texture to the slot chosen in the rendering loop
 * 4. Pass the slot number to the shader
 */

class Texture
{
private:

	int im_w = 0, im_h = 0, im_bpp = 0;
	std::string tex_path;
	GLuint tex_ID = 0;

	void _cpyInfo(const Texture& _tex);
	void _delTexture();
	void _resetTexID(GLuint _ID) { if (tex_ID > 0 && tex_ID != _ID)_delTexture(); tex_ID = _ID; }

public:

	TextureType tex_type = TextureType::NONE_TEXTURE;
	GLuint tex_slot_offset = 0;

	Texture(const std::string& texpath, TextureType tex_type, GLuint Tile_type); // using tex_type as its slot, but you can change it if you want
	Texture(GLuint Tile_type, int x, int y);									 // for FBO
	Texture(int _w, int _h, GLuint _layout, const void* _ptr,
		GLint _min_filter = GL_LINEAR, GLint _mag_filter = GL_LINEAR,
		GLint _wrap_s = GL_REPEAT, GLint _wrap_t = GL_REPEAT);				     // for generated texture
	Texture(int _w, int _h, GLuint _ID, TextureType _type, std::string _name);   // for Copy
	Texture(int _w, int _h, TextureType _type);									 // for emply texture
	Texture();

	Texture(const Texture& tex);
	Texture(Texture&& tex) noexcept;

	Texture& operator=(const Texture& tex);
	Texture& operator=(Texture&& tex)noexcept;

	~Texture();

	void Resize(const glm::vec2& size);
	void Resize(float x, float y);
	void OffsetSlot(GLuint _offset) { tex_slot_offset += _offset; };

public:
	void Bind(GLuint slot = -1) const;
	void BindC(GLuint slot = -1, GLuint read_or_write = GL_READ_WRITE, GLuint _level = 0) const;
	void BindU(GLuint slot = -1) const;
	static inline void BindM(GLuint _id, GLuint _slot = 0, TextureType _type=RGBA_TEXTURE);
	void UnbindC(GLuint slot = -1, GLuint read_or_write = GL_READ_WRITE, GLuint _level = 0) const;
	void Unbind() const;

public:
	inline int GetW()const { return im_w; }
	inline int GetH()const { return im_h; }
	inline int GetBPP()const { return im_bpp; }
	GLuint GetTexID() const { return tex_ID; }
	std::string GetTexName() const { return tex_path; }

public:
	using TexStorageInfo = const std::tuple<GLuint, GLuint, GLuint, GLuint>; // internal_layout | layout | data_type | texture_type

	inline static TexStorageInfo ParseFormat(TextureType _type);			 // internal_layout | layout | data_type | texture_type
	template<GLuint Type>
	inline static void SetTexParam(GLuint _id, GLuint _fil_min, GLuint _fil_max, GLuint _warp_s = 0, GLuint _warp_t = 0, GLuint _lev_min = 0, GLuint _lev_max = 0, GLuint _warp_r = 0);

public: // for texture processing
	void GenIrradiaceConvFrom(const Texture& _Tar_Tex);
	void GenIBLSpecularFrom(const Texture& _Tar_Tex, bool to_cubemap = false);
	void GenIBLDiffuseFrom(const Texture& _Tar_Tex, bool to_cubemap = false);
	void GenCubeMapFrom(const Texture& _Tar_Tex, size_t res = 1024);
	void GenERectMapFrom(const Texture& _Tar_Tex, size_t _w = 2048, size_t _h = 1024);
	void ConvertDepthFrom(const Texture& _Tar_Tex);
	void ConvertPNGFrom(const Texture& _Tar_Tex);

	void FillColor(const glm::vec4 col);

private:
	void GenIrradianceConv(GLuint _tar_ID, size_t _tar_w, size_t _tar_h, TextureType _tar_type = IBL_TEXTURE);
	void GenIBLSpecular(GLuint _tar_ID, size_t _tar_w, size_t _tar_h, TextureType _tar_type = IBL_TEXTURE, bool to_cubemap = false);
	void GenIBLDiffuse(GLuint _tar_ID, size_t _tar_w, size_t _tar_h, TextureType _tar_type = IBL_TEXTURE, bool to_cubemap = false);
	void GenCubeMap(GLuint _tar_ID, size_t _tar_res, TextureType _tar_type = IBL_TEXTURE);
	void GenERectMap(GLuint _tar_ID, size_t _w, size_t _h, TextureType _tar_type = IBL_TEXTURE);
	void ConvertDepth(GLuint _tar_ID, size_t _w, size_t _h, TextureType _tar_type = DEPTH_TEXTURE);
	void ConvertPNG(GLuint _tar_ID, size_t _w, size_t _h);

public:

	void SaveTexture(std::string _path) const;
};

#include <unordered_map>

class TextureLib {


public:

	static std::string root_dir;
	static TextureType ParseFileEXT(std::string path);

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
	static TextureRes LoadTexture(std::string _name);
	static GLuint GetTextureID(const std::string& _name);

public:
	static TextureRes Noise_2D_4x4();
	static TextureRes Noise_2D_16x16();

	static TextureRes IBL_LUT();

	static TextureRes LTC1();
	static TextureRes LTC2();

private:
	static void GenNoiseTexture(NoiseType _type, size_t _w, size_t _h);

};

