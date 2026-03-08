#include "Texture.h"
#include "AssetManager.h"
#include "stb_image.h"
#include "macros.h"
#include "xdz_math.h"
#include <algorithm>

void Texture::_cpyInfo(const Texture& tex)
{
	im_w = tex.GetW(); im_h = tex.GetH(); im_bpp = tex.GetBPP();
	tex_path = tex.GetTexName(); tex_ID = tex.GetTexID();
	tex_type = tex.tex_type; tex_slot_offset = tex.tex_slot_offset;
}

Texture::Texture(const std::string& texpath, TextureType tex_type, GLuint tile_type)
	:tex_path(texpath), tex_type(tex_type),
	im_bpp(0), im_h(0), im_w(0)
{
	stbi_set_flip_vertically_on_load(0);
	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	GLubyte* m_buffer = nullptr;
	GLfloat* m_buffer_f = nullptr;

	if (tex_path.find(TextureLib::root_dir) == std::string::npos)
		tex_path = TextureLib::root_dir + tex_path;

	auto [interlayout, layout, type, _] = Texture::ParseFormat(tex_type);

	switch (tex_type)
	{
	case RGBA_TEXTURE:

		m_buffer = stbi_load(tex_path.c_str(), &im_w, &im_h, &im_bpp, 4);

		Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, tile_type, tile_type, 0, 4);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
		glTexStorage2D(GL_TEXTURE_2D, 8, interlayout, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, layout, type, m_buffer);
#if 1
		glGenerateMipmap(GL_TEXTURE_2D);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#endif

		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_buffer) {
			stbi_image_free(m_buffer);
#ifdef _DEBUG
			std::cout << "Image texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
#endif
		}
		else {
			std::cout << "Image texture FAILED" << std::endl;
			_delTexture();
		}
		break;

	case HDR_TEXTURE:
		m_buffer_f = stbi_loadf(tex_path.c_str(), &im_w, &im_h, &im_bpp, 4);

		Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, tile_type, tile_type, 0, 8);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, im_w, im_h, 0, GL_RGBA, GL_FLOAT, m_buffer_f); //
		glTexStorage2D(GL_TEXTURE_2D, 8, interlayout, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, layout, type, m_buffer_f);

#if 1
		glGenerateMipmap(GL_TEXTURE_2D);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#endif

		glBindTexture(GL_TEXTURE_2D, 0);
		if (m_buffer_f) {
			stbi_image_free(m_buffer_f);

#ifdef _DEBUG
			std::cout << "HDR texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
#endif
		}
		else {
			std::cout << "HDR texture FAILED" << std::endl;
			_delTexture();
		}
		break;
	default:
		assert(false && "Unsupported texture type for file loading");
	}
}

Texture::Texture()
{

}

Texture::Texture(int _w, int _h, TextureType tex_type, GLuint tile_type)
	:tex_path(""), tex_type(tex_type),
	im_bpp(32), im_h(_h), im_w(_w)
{
	auto [interlayout, layout, type, _] = Texture::ParseFormat(tex_type);

	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);


	glTexImage2D(GL_TEXTURE_2D, 0, interlayout, im_w, im_h, 0, layout, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tile_type);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tile_type);


	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int _w, int _h, GLuint _layout, const void* _ptr,
	GLint _min_filter, GLint _mag_filter, GLint _wrap_s, GLint _wrap_t)
	:im_w(_w), im_h(_h)
{

	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, _min_filter, _mag_filter, _wrap_s, _wrap_t, 0, 1);

	switch (_layout)
	{
	case GL_RG8:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RG, GL_UNSIGNED_BYTE, _ptr);
		tex_type = RG_TEXTURE;
		break;
	case GL_RG16F:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RG, GL_FLOAT, _ptr);
		tex_type = RG_TEXTURE;
		break;
	case GL_RGB8:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RGB, GL_UNSIGNED_BYTE, _ptr);
		tex_type = RGB_TEXTURE;
		break;
	case GL_RGB16F:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RGB, GL_FLOAT, _ptr);
		tex_type = RGB_TEXTURE;
		break;
	case GL_RGBA8:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, _ptr);
		tex_type = BUFFER_TEXTURE;
		break;
	case GL_RGBA32F:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RGBA, GL_FLOAT, _ptr);
		tex_type = HDR_BUFFER_TEXTURE;
		break;
	default:
		assert(false && "WRONG LAYOUT");
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int _w, int _h, GLuint _ID, TextureType _type, std::string _name)
	:im_w(_w), im_h(_h), tex_ID(_ID), tex_type(_type), tex_path(_name)
{
}

Texture::Texture(int _w, int _h, TextureType _type)
	: im_w(_w), im_h(_h), tex_type(_type)
{
	auto [interlayout, layout, data_type, gl_type] = Texture::ParseFormat(tex_type);

	glGenTextures(1, &tex_ID);
	glBindTexture(gl_type, tex_ID);

	switch (gl_type)
	{
	case GL_TEXTURE_2D:
		Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, interlayout, im_w, im_h, 0, layout, data_type, NULL);
		break;
	case GL_TEXTURE_CUBE_MAP:
		Texture::SetTexParam<GL_TEXTURE_CUBE_MAP>(tex_ID, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0, 0, GL_CLAMP_TO_EDGE);
		LOOP(6)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, interlayout, im_w, im_w, 0, layout, data_type, NULL);
		break;
	}
}

Texture::Texture(const Texture& tex)
{
	if (this == &tex)
		return;
	if (tex_ID != 0)
		_delTexture();
	_deepCopyFrom(tex);
}

Texture::Texture(Texture&& tex) noexcept
{
	_cpyInfo(tex);
	tex.tex_ID = 0;
}

Texture& Texture::operator=(const Texture& tex)
{
	if (this == &tex)
		return *this;
	if (tex_ID != 0)
		_delTexture();
	_deepCopyFrom(tex);
	return *this;
}

Texture& Texture::operator=(Texture&& tex)noexcept
{
	if (this == &tex)
		return *this;

	_cpyInfo(tex);
	tex.tex_ID = 0;
	return *this;
}

Texture::~Texture()
{
	if (tex_ID != 0)
		_delTexture();
}

void Texture::_delTexture()
{
	glDeleteTextures(1, &tex_ID);
	tex_ID = 0;
}

void Texture::_deepCopyFrom(const Texture& tex)
{
	_cpyInfo(tex);

	if (tex.tex_ID == 0)
		return;

	auto [interlayout, layout, type, gl_type] = Texture::ParseFormat(tex_type);
	if (gl_type == GL_NONE)
		return;

	glGenTextures(1, &tex_ID);

	GLint min_filter = GL_LINEAR;
	GLint mag_filter = GL_LINEAR;
	GLint wrap_s = GL_REPEAT;
	GLint wrap_t = GL_REPEAT;
	GLint wrap_r = GL_REPEAT;
	GLint base_level = 0;
	GLint max_level = 0;
	GLint width = 0;
	GLint height = 0;
	GLint depth = 1;
	GLint src_internal = 0;
	GLint dst_internal = 0;
	GLint immutable_levels = 0;

	glBindTexture(gl_type, tex.tex_ID);
	glGetTexParameteriv(gl_type, GL_TEXTURE_MIN_FILTER, &min_filter);
	glGetTexParameteriv(gl_type, GL_TEXTURE_MAG_FILTER, &mag_filter);
	glGetTexParameteriv(gl_type, GL_TEXTURE_WRAP_S, &wrap_s);
	glGetTexParameteriv(gl_type, GL_TEXTURE_WRAP_T, &wrap_t);
	glGetTexParameteriv(gl_type, GL_TEXTURE_WRAP_R, &wrap_r);
	glGetTexParameteriv(gl_type, GL_TEXTURE_BASE_LEVEL, &base_level);
	glGetTexParameteriv(gl_type, GL_TEXTURE_MAX_LEVEL, &max_level);
	glGetTexParameteriv(gl_type, GL_TEXTURE_IMMUTABLE_LEVELS, &immutable_levels);
	glGetTexLevelParameteriv(gl_type, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(gl_type, 0, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(gl_type, 0, GL_TEXTURE_DEPTH, &depth);
	glGetTexLevelParameteriv(gl_type, 0, GL_TEXTURE_INTERNAL_FORMAT, &src_internal);
	assert(im_w == width);
	assert(im_h == height);

	int levels = std::min(std::max(immutable_levels, 1), max_level + 1);

	glBindTexture(gl_type, tex_ID);
	glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(gl_type, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(gl_type, GL_TEXTURE_MAX_LEVEL, levels - 1);
	
	if (gl_type == GL_TEXTURE_2D) {
		glTexStorage2D(gl_type, levels, interlayout, width, height);
		glGetTexLevelParameteriv(gl_type, 0, GL_TEXTURE_INTERNAL_FORMAT, &dst_internal);
		for (int level = 0; level < levels; ++level) {
			int copy_w = std::max(1, width >> level);
			int copy_h = std::max(1, height >> level);
			glCopyImageSubData(
				tex.tex_ID, gl_type, level,
				0, 0, 0,
				tex_ID, gl_type, level,
				0, 0, 0,
				copy_w, copy_h, 1);
		}
	}
	else if (gl_type == GL_TEXTURE_CUBE_MAP) {
		glTexParameteri(gl_type, GL_TEXTURE_WRAP_R, wrap_r);
		glTexStorage2D(gl_type, levels, interlayout, width, height);
		glGetTexLevelParameteriv(gl_type, 0, GL_TEXTURE_INTERNAL_FORMAT, &dst_internal);
		for (int level = 0; level < levels; ++level) {
			int copy_w = std::max(1, width >> level);
			int copy_h = std::max(1, height >> level);
			glCopyImageSubData(tex.tex_ID, gl_type, level, 0, 0, 0,
				tex_ID, gl_type, level, 0, 0, 0,
				copy_w, copy_h, 6);
		}
	}
	else if (gl_type == GL_TEXTURE_2D_ARRAY) {
		glTexParameteri(gl_type, GL_TEXTURE_WRAP_R, wrap_r);
		glTexStorage3D(gl_type, levels, interlayout, width, height, depth);
		glGetTexLevelParameteriv(gl_type, 0, GL_TEXTURE_INTERNAL_FORMAT, &dst_internal);
		for (int level = 0; level < levels; ++level) {
			int copy_w = std::max(1, width >> level);
			int copy_h = std::max(1, height >> level);
			int copy_d = std::max(1, depth >> level);
			glCopyImageSubData(tex.tex_ID, gl_type, level, 0, 0, 0,
				tex_ID, gl_type, level, 0, 0, 0,
				copy_w, copy_h, copy_d);
		}
	}

	glBindTexture(gl_type, 0);
}

void Texture::Resize(const glm::vec2& size)
{
	Resize((GLuint)size.x, (GLuint)size.y);
}

void Texture::Resize(GLuint x, GLuint y)
{
	if(im_w == (int)x && im_h == (int)y)
		return;

	im_w = x;
	im_h = y;

	auto [interlayout, layout, type, gl_type] = Texture::ParseFormat(tex_type);
	glBindTexture(gl_type, tex_ID);

	if (gl_type == GL_TEXTURE_2D) {
		glTexImage2D(gl_type, 0, interlayout, im_w, im_h, 0, layout, type, NULL);
		Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_LINEAR, GL_LINEAR);
	}
	else if (gl_type == GL_TEXTURE_CUBE_MAP) {
		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				interlayout, // internal format
				im_w,
				im_h,
				0,
				layout, type,
				nullptr
			);
		}
		Texture::SetTexParam<GL_TEXTURE_CUBE_MAP>(tex_ID, GL_LINEAR, GL_LINEAR);
	}
}

void Texture::Bind(GLuint slot) const
{
	if (slot == -1)
		slot = tex_type + tex_slot_offset;

	Texture::BindM(tex_ID, slot, tex_type);
}

void Texture::BindC(GLuint slot /*= -1*/, GLuint read_or_write /*= GL_READ_WRITE*/, GLuint _level /*= 0*/) const
{
	if (slot == -1)
		slot = tex_type + tex_slot_offset;

	auto [layout, _1, _2, gl_type] = Texture::ParseFormat(tex_type);
	GLuint is_array = (_level != 0 || gl_type == GL_TEXTURE_CUBE_MAP) ? GL_TRUE : GL_FALSE;

	glBindImageTexture(slot, tex_ID, 0, is_array, 0, read_or_write, layout);
}

void Texture::BindU(GLuint slot /*= -1*/) const
{
	if (slot == -1) slot = tex_type;

	glBindTextureUnit(slot, tex_ID);
}

inline void Texture::BindM(GLuint _id, GLuint _slot /*= 0*/, TextureType _type/*=RGBA_TEXTURE*/)
{
	const GLuint gl_type = std::get<3>(Texture::ParseFormat(_type));
	glActiveTexture(GL_TEXTURE0 + _slot);
	glBindTexture(gl_type, _id);
}

void Texture::UnbindC(GLuint slot /*= -1*/, GLuint read_or_write /*= GL_READ_WRITE*/, GLuint _level /*= 0*/) const
{
	if (slot == -1)
		slot = tex_type + tex_slot_offset;


	// other parameters are not necessary.
	glBindImageTexture(slot, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void Texture::Unbind() const
{
	glBindTexture(std::get<3>(Texture::ParseFormat(tex_type)), 0);
	//glActiveTexture(0);
}

inline Texture::TexStorageInfo Texture::ParseFormat(TextureType _type)
{
	// https://docs.gl/gl4/glTexImage2D

	switch (_type)
	{
	case RGBA_TEXTURE:
	case BUFFER_TEXTURE:
		return { GL_RGBA8,				GL_RGBA,			GL_UNSIGNED_BYTE,	GL_TEXTURE_2D };
	case RGB_TEXTURE:
		return { GL_RGB8,				GL_RGB,				GL_UNSIGNED_BYTE,	GL_TEXTURE_2D };
	case HDR_TEXTURE:
	case HDR_BUFFER_TEXTURE:
		return { GL_RGBA32F,			GL_RGBA,			GL_FLOAT,			GL_TEXTURE_2D };
	case HDR_CUBE_TEXTURE:
		return { GL_RGBA32F,			GL_RGBA,			GL_FLOAT,			GL_TEXTURE_CUBE_MAP };
	case FLOAT_BUFFER_TEXTURE:
	case RG_TEXTURE:
		return { GL_RG16F,				GL_RG,				GL_FLOAT,			GL_TEXTURE_2D };
	case LAYERED_TEXTURE:
		return { GL_RGBA32F,			GL_RGBA,			GL_FLOAT,			GL_TEXTURE_2D_ARRAY };
	case LIGHTING_CACHE:
		return { GL_R16F,				GL_RED,				GL_FLOAT,			GL_TEXTURE_2D };
	case DEPTH_CUBE_TEXTURE:
		return { GL_DEPTH_COMPONENT32F,	GL_DEPTH_COMPONENT, GL_FLOAT,			GL_TEXTURE_CUBE_MAP };
	case DEPTH_TEXTURE:
		return { GL_DEPTH_COMPONENT32F,	GL_DEPTH_COMPONENT, GL_FLOAT,			GL_TEXTURE_2D };
	default:
		//assert(false && "WRONG FORMAT");
		return { GL_NONE,				GL_NONE ,			GL_NONE,			GL_NONE };
	}
}

template<GLuint Type>
inline void Texture::SetTexParam(GLuint _id, GLuint _fil_min, GLuint _fil_max, GLuint _warp_s /*= 0*/, GLuint _warp_t /*= 0*/, GLuint _lev_min /*= 0*/, GLuint _lev_max /*= 0*/, GLuint _warp_r /*= 0*/)
{
	glBindTexture(Type, _id);
	
	glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, _fil_min);
	glTexParameteri(Type, GL_TEXTURE_MAG_FILTER, _fil_max);
	
	if (_warp_s * _warp_t != 0) {
		glTexParameteri(Type, GL_TEXTURE_WRAP_S, _warp_s);
		glTexParameteri(Type, GL_TEXTURE_WRAP_T, _warp_t);
	}
	
	if (_warp_r != 0)
		glTexParameteri(Type, GL_TEXTURE_WRAP_T, _warp_r);
	
	if (_lev_min + _lev_max != 0) {
		glTexParameteri(Type, GL_TEXTURE_BASE_LEVEL, _lev_min);
		glTexParameteri(Type, GL_TEXTURE_MAX_LEVEL, _lev_max);
	}
}

#include "shaders/ComputeShader.h"
#include "LtcMatrix.h"

void Texture::GenIrradiaceConvFrom(const Texture& _Tar_Tex)
{
	GenIrradianceConv(_Tar_Tex.GetTexID(), _Tar_Tex.im_w, _Tar_Tex.im_h, _Tar_Tex.tex_type);
}

void Texture::GenIBLDiffuseFrom(const Texture& _Tar_Tex, bool to_cubemap /*= false*/)
{
	GenIBLDiffuse(_Tar_Tex.GetTexID(), _Tar_Tex.im_w, _Tar_Tex.im_h, _Tar_Tex.tex_type, to_cubemap);
}

void Texture::GenIBLSpecularFrom(const Texture& _Tar_Tex, bool to_cubemap /*= false*/)
{
	GenIBLSpecular(_Tar_Tex.GetTexID(), _Tar_Tex.im_w, _Tar_Tex.im_h, _Tar_Tex.tex_type, to_cubemap);
}

void Texture::GenCubeMapFrom(const Texture& _Tar_Tex, int res)
{
	GenCubeMap(_Tar_Tex.GetTexID(), res, _Tar_Tex.tex_type);
}

void Texture::GenERectMapFrom(const Texture& _Tar_Tex, int _w /*= 2048*/, int _h /*= 1024*/)
{
	GenERectMap(_Tar_Tex.GetTexID(), _w, _h, _Tar_Tex.tex_type);
}

void Texture::ConvertDepthFrom(const Texture& _Tar_Tex)
{
	ConvertDepth(_Tar_Tex.GetTexID(), _Tar_Tex.GetW(), _Tar_Tex.GetH(), _Tar_Tex.tex_type);
}

void Texture::ConvertDepthCubeFrom(const Texture& _Tar_Tex)
{
	ConvertDepthCube(_Tar_Tex.GetTexID(), _Tar_Tex.GetW(), _Tar_Tex.GetH(), _Tar_Tex.tex_type);
}

void Texture::ConvertPNGFrom(const Texture& _Tar_Tex)
{
	ConvertPNG(_Tar_Tex.GetTexID(), _Tar_Tex.GetW(), _Tar_Tex.GetH());
}

void Texture::FillColor(const glm::vec4 col)
{
	ComputeShader& fill = ComputeShader::ImportShader("pps/Fill");
	GLuint slot = -1;
	auto [interlayout, layout, type, _] = Texture::ParseFormat(tex_type);

	switch (interlayout)
	{
	case GL_RGBA32F:	slot = 0; break;
	case GL_RGBA8:		slot = 1; break;
	case GL_R16F:		slot = 2; break;
	default: assert(false && "unknown format");
	}

	BindC(slot);
	fill.UseShader();
	fill.SetValue("color", col);
	fill.SetValue("slot", slot);
	fill.RunComputeShaderSCR({ im_w, im_h }, 4);
}

void Texture::GenIrradianceConv(GLuint _tar_ID, int _tar_w, int _tar_h, TextureType _tar_type /*= HDR_TEXTURE*/)
{
	assert(false && "this function has been abandoned");
}

void Texture::GenIBLSpecular(GLuint _tar_ID, int _tar_w, int _tar_h, TextureType _tar_type /*= HDR_TEXTURE*/, bool to_cubemap /*= false*/)
{
	//////////////////   Specular Importance Sampling   //////////////////
	//			https://learnopengl.com/PBR/IBL/Specular-IBL			//

	const int max_inter = 8;

	GLuint ID;
	glGenTextures(1, &ID);		//for storage

	if (to_cubemap) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
		Texture::SetTexParam<GL_TEXTURE_CUBE_MAP>(ID, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0, max_inter - 1, GL_CLAMP_TO_EDGE);
		im_w = im_h = 2048; tex_type = HDR_CUBE_TEXTURE;
	}
	else {
		glBindTexture(GL_TEXTURE_2D, ID);
		Texture::SetTexParam<GL_TEXTURE_2D>(ID, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_MIRRORED_REPEAT, 0, max_inter - 1);
		im_w = _tar_w; im_h = _tar_h; tex_type = _tar_type;
	}

	auto [interlayout, layout, type, _] = Texture::ParseFormat(_tar_type);

	LOOP(8) {
		int lod_w = im_w / (int)pow(2, i);
		int lod_h = im_h / (int)pow(2, i);

		lod_w -= lod_w % 4;
		lod_h -= lod_h % 4;

		if (to_cubemap) {
			LOOP_N(6, s)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + s, i, interlayout, lod_w, lod_h, 0, layout, type, NULL);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, i, interlayout, lod_w, lod_h, 0, layout, type, NULL);
		}
	}

	ComputeShader& importance_samp = ComputeShader::ImportShader(to_cubemap ? "convert/Importance_Samp_E2C" : "convert/Importance_Samp");
	LOOP(8) { // 0 1 2 3 4 5 6 7
		int lod_w = im_w / (int)pow(2, i);
		int lod_h = im_h / (int)pow(2, i);

		lod_w -= lod_w % 4;
		lod_h -= lod_h % 4;

		Texture::BindM(_tar_ID, _tar_type);
		glBindImageTexture(tex_type + 1, ID, i, (GLuint)to_cubemap, 0, GL_WRITE_ONLY, interlayout); // HDR_TEXTURE -> HDR_TEXTURE + 1

		importance_samp.UseShader();
		importance_samp.SetValue("s", (float)i / (float)(max_inter - 1));
		importance_samp.SetValue("max_step", 32);
		importance_samp.SetValue("source", _tar_type);
		importance_samp.RunComputeShader(lod_w / 4, lod_h / 4, to_cubemap ? 6 : 1);
	}

	_resetTexID(ID);
}

void Texture::GenIBLDiffuse(GLuint _tar_ID, int _tar_w, int _tar_h, TextureType _tar_type /*= HDR_TEXTURE*/, bool to_cubemap /*= false*/)
{
	//////////////////  Diffuse Irradience Convolution  //////////////////
	//       https://learnopengl.com/PBR/IBL/Diffuse-irradiance         //

	GLuint ID;
	glGenTextures(1, &ID);		//for storage
	if (to_cubemap) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
		Texture::SetTexParam<GL_TEXTURE_CUBE_MAP>(ID, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0, 0, GL_CLAMP_TO_EDGE);
		im_w = im_h = 64; tex_type = HDR_CUBE_TEXTURE;
	}
	else {
		glBindTexture(GL_TEXTURE_2D, ID);
		Texture::SetTexParam<GL_TEXTURE_2D>(ID, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_MIRRORED_REPEAT);
		im_w = _tar_w / 8; im_h = _tar_h / 8; tex_type = _tar_type;
	}

	auto [interlayout, layout, type, _] = Texture::ParseFormat(_tar_type);
	if (to_cubemap) {
		LOOP_N(6, s)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + s, 0, interlayout, im_w, im_h, 0, layout, type, NULL);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, interlayout, im_w, im_h, 0, layout, type, NULL);
	}

	Texture::BindM(_tar_ID, _tar_type + 1);
	glBindImageTexture(tex_type, ID, 0, (GLuint)to_cubemap, 0, GL_WRITE_ONLY, interlayout);

	ComputeShader& irradiance_conv = ComputeShader::ImportShader(to_cubemap ? "convert/Irradiance_Conv_E2C" : "convert/Irradiance_Conv");
	irradiance_conv.UseShader();
	irradiance_conv.SetValue("max_step", 64);
	irradiance_conv.SetValue("source", _tar_type + 1);
	irradiance_conv.RunComputeShader(im_w / 4, im_w / 4, to_cubemap ? 6 : 1);

	_resetTexID(ID);
}

void Texture::GenCubeMap(GLuint _tar_ID, int _tar_res, TextureType _tar_type /*= HDR_TEXTURE*/)
{
	const bool type_correct = !((_tar_type == HDR_CUBE_TEXTURE) || (_tar_type == DEPTH_CUBE_TEXTURE));
	assert(type_correct && "Wrong input texture type");

	// https://learnopengl.com/Advanced-OpenGL/Cubemaps

	auto [interlayout, layout, type, _] = Texture::ParseFormat(_tar_type);

	GLuint ID;
	glGenTextures(1, &ID);		//for storage
	Texture::SetTexParam<GL_TEXTURE_CUBE_MAP>(ID, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0, 0, GL_CLAMP_TO_EDGE);

	LOOP(6)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, interlayout, _tar_res, _tar_res, 0, layout, type, NULL);

	ComputeShader& to_cubemap = ComputeShader::ImportShader("convert/E2C", Uni("U_etangular", 1));

	glBindImageTexture(0, ID, 0, GL_TRUE, 0, GL_WRITE_ONLY, interlayout);
	Texture::BindM(_tar_ID, 1);
	to_cubemap.UseShader();
	to_cubemap.RunComputeShader(_tar_res / 4, _tar_res / 4, 6);
	_resetTexID(ID);

	tex_type = HDR_CUBE_TEXTURE;
	im_w = im_h = _tar_res;
}

void Texture::GenERectMap(GLuint _tar_ID, int _w, int _h, TextureType _tar_type /*= HDR_TEXTURE*/)
{
	const bool type_correct = (_tar_type == HDR_CUBE_TEXTURE) || (_tar_type == DEPTH_CUBE_TEXTURE);
	assert(type_correct && "Wrong input texture type");

	auto [interlayout, layout, type, _] = Texture::ParseFormat(HDR_CUBE_TEXTURE);

	GLuint ID;
	glGenTextures(1, &ID);		//for storage
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, interlayout, _w, _h, 0, layout, type, NULL);
	Texture::SetTexParam<GL_TEXTURE_2D>(ID, GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

	ComputeShader& to_rectmap = ComputeShader::ImportShader("convert/C2E", Uni("U_Cube", 1));

	glBindImageTexture(0, ID, 0, GL_FALSE, 0, GL_WRITE_ONLY, interlayout);
	Texture::BindM(_tar_ID, 1, _tar_type);
	to_rectmap.UseShader();
	to_rectmap.RunComputeShader(_w / 4, _h / 4);

	_resetTexID(ID);

	tex_type = HDR_TEXTURE;
	im_w = _w; im_h = _h;
}

void Texture::ConvertDepth(GLuint _tar_ID, int _w, int _h, TextureType _tar_type /*= DEPTH_TEXTURE*/)
{

	const bool type_correct = (_tar_type == DEPTH_TEXTURE) || (_tar_type == DEPTH_CUBE_TEXTURE);
	if (!type_correct) return;

	auto [interlayout, layout, type, gl_type] = Texture::ParseFormat(_tar_type);

	GLuint ID;
	glGenTextures(1, &ID);		//for storage
	Texture::SetTexParam<GL_TEXTURE_2D>(ID, GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _w, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	ComputeShader& to_texture = ComputeShader::ImportShader(gl_type == GL_TEXTURE_2D ? "convert/Depth_Texture" : "convert/Depth_Texture_C2E");

	Texture::BindM(_tar_ID, 0, _tar_type);
	glBindImageTexture(1, ID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	to_texture.UseShader();
	to_texture.SetValue("U_depth", 0);
	to_texture.RunComputeShader(_w / 4, _h / 4);

	_resetTexID(ID);

	tex_type = RGBA_TEXTURE;
	im_w = _w; im_h = _h;
}

void Texture::ConvertDepthCube(GLuint _tar_ID, int _w, int _h, TextureType _tar_type /*= DEPTH_CUBE_TEXTURE*/)
{
	if (_tar_type != DEPTH_CUBE_TEXTURE) return;

	GLuint ID;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	Texture::SetTexParam<GL_TEXTURE_CUBE_MAP>(ID, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0, 0, GL_CLAMP_TO_EDGE);

	LOOP(6)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, _w, _h, 0, GL_RGBA, GL_FLOAT, nullptr);

	ComputeShader& to_texture = ComputeShader::ImportShader("convert/Depth_Texture_C2C");

	Texture::BindM(_tar_ID, 0, _tar_type);
	glBindImageTexture(1, ID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	to_texture.UseShader();
	to_texture.SetValue("U_depth", 0);
	to_texture.RunComputeShader((_w + 3) / 4, (_h + 3) / 4, 6);

	_resetTexID(ID);

	tex_type = HDR_CUBE_TEXTURE;
	im_w = _w; im_h = _h;
}

void Texture::ConvertPNG(GLuint _tar_ID, int _w, int _h)
{
	auto [interlayout, layout, type, _] = Texture::ParseFormat(RGBA_TEXTURE);

	GLuint ID;
	glGenTextures(1, &ID);		//for storage
	Texture::SetTexParam<GL_TEXTURE_2D>(ID, GL_LINEAR, GL_LINEAR, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, interlayout, _w, _h, 0, layout, type, NULL);

	ComputeShader& hdr_to_png = ComputeShader::ImportShader("convert/HDR2PNG");

	glBindImageTexture(0, ID, 0, GL_FALSE, 0, GL_WRITE_ONLY, interlayout);
	glBindImageTexture(1, _tar_ID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);;

	hdr_to_png.UseShader();
	hdr_to_png.SetValue("U_HDR", 1);
	hdr_to_png.RunComputeShader(_w, _h);

	_resetTexID(ID);

	tex_type = RGBA_TEXTURE;
	im_w = _w; im_h = _h;
}

#include "stb_image_write.h"
void Texture::SaveTexture(std::string _path, bool force_png, bool force_cube) const
{
	auto [_, layout, type, gl_type] = Texture::ParseFormat(tex_type);
	static std::string root = "result/";
	int status = -1;
	stbi_flip_vertically_on_write(0);
	glBindTexture(gl_type, tex_ID);
	if (gl_type == GL_TEXTURE_CUBE_MAP && !force_cube) {
		Texture rect_map;
		if (layout == GL_DEPTH_COMPONENT) {
			Texture depth_cube;
			depth_cube.ConvertDepthCubeFrom(*this);
			rect_map.GenERectMapFrom(depth_cube, depth_cube.GetW() * 2, depth_cube.GetH());
		} else {
			rect_map.GenERectMapFrom(*this, im_w * 2, im_h);
		}
		rect_map.SaveTexture(_path, force_png, true);
		return;
	}
	if (layout == GL_DEPTH_COMPONENT) {		// Depth Texture
		if (force_png) {
			static Texture depth_png;
			depth_png.ConvertDepthFrom(*this);
			depth_png.SaveTexture(_path, true);
			return;
		}

		if (gl_type == GL_TEXTURE_2D) {
			auto depth = std::vector<GLfloat>(im_w * im_h);
			auto odata = std::vector<GLfloat>(im_w * im_h * 4);
			glGetTexImage(gl_type, 0, layout, type, depth.data());
			for (size_t i = 0; i < depth.size(); ++i) {
				const size_t base = i * 4;
				odata[base] = depth[i];
				odata[base + 1] = depth[i];
				odata[base + 2] = depth[i];
				odata[base + 3] = 1.0f;
			}

			std::string outputPath = root + _path + ".hdr";
			status = stbi_write_hdr(outputPath.c_str(), im_w, im_h, 4, odata.data());
		}
		else if (gl_type == GL_TEXTURE_CUBE_MAP) {
			Texture depth_cube;
			depth_cube.ConvertDepthCubeFrom(*this);
			depth_cube.SaveTexture(_path, false, true);
		}
		else {
			assert(false && "No depth map format matched");
		}
	}else if (type == GL_FLOAT) {
		if (force_png) {
			static Texture hdr_png;
			hdr_png.ConvertPNGFrom(*this);
			hdr_png.SaveTexture(_path);
			return;
		}
		else if (gl_type == GL_TEXTURE_2D) {
			auto odata = std::vector<GLfloat>(im_w * im_h * 4);

			glGetTexImage(gl_type, 0, layout, type, odata.data());

			std::string outputPath = root + _path + ".hdr";
			status = stbi_write_hdr(outputPath.c_str(), im_w, im_h, 4, odata.data());
		}
		else if (gl_type == GL_TEXTURE_CUBE_MAP) {
			auto odata = std::vector<GLfloat>(im_w * im_h * 4);
			LOOP(6) {
				glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, layout, type, odata.data());

				// e.g. outputPath = result/hdr_cube/hdr_cube_1.hdr
				std::string outputPath = root + _path + "/" + _path + "_" + std::to_string(i + 1) + ".hdr";
				status = stbi_write_hdr(outputPath.c_str(), im_w, im_h, 4, odata.data());
			}
		}
		else {
			assert(false && "No HDR map format matched");
		}
	}
	else if (type == GL_UNSIGNED_BYTE) {
		if (gl_type == GL_TEXTURE_2D) {
			assert(type == GL_UNSIGNED_BYTE);
			auto odata = std::vector<GLbyte>(im_w * im_h * 4);

			glGetTexImage(gl_type, 0, layout, type, odata.data());

			std::string outputPath = root + _path + ".png";
			status = stbi_write_png(outputPath.c_str(), im_w, im_h, 4, odata.data(), 0);
		}
		else if (gl_type == GL_TEXTURE_CUBE_MAP) {
			assert(type == GL_UNSIGNED_BYTE);
			auto odata = std::vector<GLbyte>(im_w * im_h * 4);

			LOOP(6) {
				glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, layout, type, odata.data());

				std::string outputPath = root + _path + "/" + _path + "_" + std::to_string(i + 1) + ".png";
				status = stbi_write_png(outputPath.c_str(), im_w, im_h, 4, odata.data(), 0);
			}
		}
		else {
			assert(false && "No bit map format matched");
		}
	}
	else {
		assert(false && "No texture type matched");
	}

	if (status == 0) {
		DEBUG("failed");
	}
}

void Texture::PrintTexture() const
{
	auto [_, layout, type, gl_type] = Texture::ParseFormat(tex_type);

	auto odata = std::vector<float>(im_w * im_h * 4);

	glBindTexture(gl_type, tex_ID);
	glGetTexImage(gl_type, 0, layout, type, odata.data());

	int a = 0;
}




///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

std::string TextureLib::root_dir = "res/tex/";

Texture::TextureType TextureLib::ParseFileEXT(std::string path)
{
	if (path.find(".png") != std::string::npos)	return Texture::RGBA_TEXTURE;
	else if (path.find(".jpg") != std::string::npos)	return Texture::RGB_TEXTURE;
	else if (path.find(".hdr") != std::string::npos)	return Texture::HDR_TEXTURE;
	return Texture::NONE_TEXTURE;
}

std::shared_ptr<Texture> TextureLib::GetTexture(const std::string& _name)
{
	return AssetManager::Get<Texture>(_name);
}

TextureLib::TextureRes TextureLib::LoadTexture(std::string _name)
{
	return AssetManager::Load<Texture>(_name, [_name]() {
		Texture::TextureType _type = TextureLib::ParseFileEXT(_name);
		return std::make_shared<Texture>(_name, _type, GL_REPEAT);
	});
}

GLuint TextureLib::GetTextureID(const std::string& _name)
{
	return TextureLib::GetTexture(_name)->GetTexID();
}

void TextureLib::ResetTexLib()
{
	AssetManager::Clear<Texture>();
}

TextureLib::TextureRes TextureLib::Noise_2D_4x4()
{
	const std::string _name = "Uni.2D.4.4";
	auto result = GetTexture(_name);

	if (result != nullptr)
		return result;

	GenNoiseTexture(UNI_2D_NOISE, 4, 4);

	return AssetManager::Get<Texture>(_name);
}

TextureLib::TextureRes TextureLib::Noise_2D_4x4xN(int n/*=6*/)
{
	const std::string _name = "Uni.2D.4.4." + std::to_string(n);
	auto result = GetTexture(_name);

	if (result != nullptr)
		return result;

	GenNoiseTextures(UNI_2D_NOISE, 4, 4, n);

	return AssetManager::Get<Texture>(_name);
}

TextureLib::TextureRes TextureLib::Noise_2D_16x16xN(int n/*=6*/)
{
	const std::string _name = "Uni.2D.16.16." + std::to_string(n);
	auto result = GetTexture(_name);

	if (result != nullptr)
		return result;

	GenNoiseTextures(UNI_2D_NOISE, 16, 16, n);

	return AssetManager::Get<Texture>(_name);
}

TextureLib::TextureRes TextureLib::IBL_LUT()
{
	const std::string _name = "IBL_LUT";
	auto result = GetTexture(_name);

	if (result != nullptr)
		return result;

	AssetManager::Register<Texture>(_name, std::make_shared<Texture>("ibl_brdf_lut.png", Texture::RGBA_TEXTURE, GL_CLAMP));

	return AssetManager::Get<Texture>(_name);
}

TextureLib::TextureRes TextureLib::LTC1()
{
	const std::string _name = "LTC1";
	auto result = GetTexture(_name);

	if (result != nullptr) return result;

	AssetManager::Register<Texture>(_name, std::make_shared<Texture>(64, 64, GL_RGBA32F, reinterpret_cast<const void*>(LTC1_DATA), GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE));

	return AssetManager::Get<Texture>(_name);
}

TextureLib::TextureRes TextureLib::LTC2()
{
	const std::string _name = "LTC2";
	auto result = GetTexture(_name);

	if (result != nullptr) return result;

	AssetManager::Register<Texture>(_name, std::make_shared<Texture>(64, 64, GL_RGBA32F, reinterpret_cast<const void*>(LTC2_DATA), GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE));

	return AssetManager::Get<Texture>(_name);
}

void TextureLib::GenNoiseTexture(NoiseType _type, int _w, int _h)
{
	GLuint id;

	glGenTextures(1, &id);//for storage
	glBindTexture(GL_TEXTURE_2D, id);

	Texture::SetTexParam<GL_TEXTURE_2D>(id, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

	std::vector<glm::vec4> list(_w * _h);
	LOOP(_w * _h) list[i] = xdzm::rand4();
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, _w, _h);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _w, _h, GL_RGBA, GL_FLOAT, list.data());

	std::string type_name;
	int dimension = 4;

	switch (_type)
	{
	case TextureLib::NONE_NOISE:
		type_name = "Non";
		break;
	case TextureLib::UNIFORM_NOISE:
		type_name = "Uni";
		break;
	case TextureLib::GAUSSIAN_NOISE:
		type_name = "Gau";
		break;
	case TextureLib::UNI_2D_NOISE:
		type_name = "Uni";
		dimension = 2;
		break;
	default:
		break;
	}

	std::string noise_name = type_name + "." + std::to_string(dimension) + "D." + std::to_string(_w) + "." + std::to_string(_h);

	AssetManager::Register<Texture>(noise_name, std::make_shared<Texture>(_w, _h, id, Texture::HDR_BUFFER_TEXTURE, noise_name));
}

void TextureLib::GenNoiseTextures(NoiseType _type, int _w, int _h, int _n)
{
	GLuint id;

	glGenTextures(1, &id);//for storage
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);

	Texture::SetTexParam<GL_TEXTURE_2D_ARRAY>(id, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0, _n);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, _w, _h, _n);

	LOOP_N(_n, level) {
		std::vector<glm::vec4> list(_w * _h);
		LOOP(_w * _h) list[i] = xdzm::rand4();
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, level, _w, _h, 1, GL_RGBA, GL_FLOAT, (float*)list.data());
	}

	std::string type_name;
	int dimension = 4;

	switch (_type)
	{
	case TextureLib::NONE_NOISE:
		type_name = "Non";
		break;
	case TextureLib::UNIFORM_NOISE:
		type_name = "Uni";
		break;
	case TextureLib::GAUSSIAN_NOISE:
		type_name = "Gau";
		break;
	case TextureLib::UNI_2D_NOISE:
		type_name = "Uni";
		dimension = 2;
		break;
	default:
		break;
	}

	std::string noise_name = type_name + "." + std::to_string(dimension) + "D." + std::to_string(_w) + "." + std::to_string(_h) + "." + std::to_string(_n);

	AssetManager::Register<Texture>(noise_name, std::make_shared<Texture>(_w, _h, id, Texture::LAYERED_TEXTURE, noise_name));
}
