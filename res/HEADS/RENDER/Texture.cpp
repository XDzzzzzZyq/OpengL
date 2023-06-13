#include "Texture.h"
#include "stb_image/stb_image.h"

std::string Texture::root_dir = "res/tex/";

Texture::Texture(const std::string& texpath, TextureType tex_type, GLuint Tile_type)
	:tex_path(texpath), tex_type(tex_type),
	im_bpp(0), im_h(0), im_w(0)
{

	GLfloat maxAnti;
	//std::cout << Tex_ID << std::endl;
	stbi_set_flip_vertically_on_load(1);
	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	GLubyte* m_buffer = nullptr;
	GLfloat* m_buffer_f = nullptr;

	if (tex_path.find(Texture::root_dir) == std::string::npos)
		tex_path = Texture::root_dir + tex_path;

	auto [interlayout, layout, type, _] = Texture::ParseFormat(tex_type);

	switch (tex_type)
	{
	case RGBA_TEXTURE:

		m_buffer = stbi_load(tex_path.c_str(), &im_w, &im_h, &im_bpp, 4);

		Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, Tile_type, Tile_type, 0, 4);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, interlayout, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, layout, type, m_buffer);

#if 1
		glGenerateMipmap(GL_TEXTURE_2D);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnti);
		glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY, maxAnti);
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
		}
		break;

	case SPIRIT_TEXURE:
		stbi_set_flip_vertically_on_load(0);
		m_buffer = stbi_load(tex_path.c_str(), &im_w, &im_h, &im_bpp, 4);

		Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_LINEAR, GL_LINEAR, Tile_type, Tile_type, 0, 1);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, interlayout, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, layout, type, m_buffer);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_buffer) {
			stbi_image_free(m_buffer);
#ifdef _DEBUG
			std::cout << "Image texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
#endif
		}
		else {
			std::cout << "Image texture FAILED" << std::endl;
		}
		break;

	case IBL_TEXTURE:
		m_buffer_f = stbi_loadf(tex_path.c_str(), &im_w, &im_h, &im_bpp, 4);

		Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, Tile_type, Tile_type, 0, 8);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, im_w, im_h, 0, GL_RGBA, GL_FLOAT, m_buffer_f); //
		glTexStorage2D(GL_TEXTURE_2D, 8, interlayout, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, layout, type, m_buffer_f);

#if 1
		glGenerateMipmap(GL_TEXTURE_2D);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnti);
		glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY, maxAnti);
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
		else
			std::cout << "HDR texture FAILED" << std::endl;
		break;

	case BUFFER_TEXTURE:
	case HDR_BUFFER_TEXTURE:
	case FLOAT_BUFFER_TEXTURE:

		glTexImage2D(GL_TEXTURE_2D, 0, interlayout, SCREEN_W, SCREEN_H, 0, layout, type, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);
		break;
	}


}

Texture::Texture()
{

}

Texture::Texture(GLuint Tile_type, int x, int y)
	:tex_path(NULL), tex_type(BUFFER_TEXTURE),
	im_bpp(8), im_h(y), im_w(x)
{
	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int _w, int _h, GLuint _layout, void* _ptr)
	:im_w(_w), im_h(_h)
{

	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0, 1);

	switch (_layout)
	{
	case GL_RG8:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RG, GL_UNSIGNED_BYTE, (GLubyte*)_ptr);
		tex_type = RG_TEXTURE;
		break;
	case GL_RG16F:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RG, GL_FLOAT, (GLfloat*)_ptr);
		tex_type = RG_TEXTURE;
		break;
	case GL_RGB8:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*)_ptr);
		tex_type = RGB_TEXTURE;
		break;
	case GL_RGB16F:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RGB, GL_FLOAT, (GLfloat*)_ptr);
		tex_type = RGB_TEXTURE;
		break;
	case GL_RGBA8:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)_ptr);
		tex_type = BUFFER_TEXTURE;
		break;
	case GL_RGBA16F:
		glTexImage2D(GL_TEXTURE_2D, 0, _layout, im_w, im_h, 0, GL_RGBA, GL_FLOAT, (GLfloat*)_ptr);
		tex_type = HDR_BUFFER_TEXTURE;
		break;
	default:
	    assert(false && "WRONG LAYOUT");
	}


}

Texture::Texture(int _w, int _h, GLuint _ID, TextureType _type, std::string _name)
	:im_w(_w), im_h(_h), tex_ID(_ID), tex_type(_type), tex_path(_name)
{

}

Texture::~Texture()
{

}

void Texture::DelTexture() const
{
	glDeleteTextures(1, &tex_ID);
}

void Texture::Resize(const ImVec2& size)
{
	Resize(size.x, size.y);
}

void Texture::Resize(float x, float y)
{
	im_h = y;
	im_w = x;
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	auto [interlayout, layout, type, _] = Texture::ParseFormat(tex_type);

	glTexImage2D(GL_TEXTURE_2D, 0, interlayout, im_w, im_h, 0, layout, type, NULL);

	Texture::SetTexParam<GL_TEXTURE_2D>(tex_ID, GL_NEAREST, GL_NEAREST);
}

void Texture::Bind(GLuint slot) const
{
	if (slot == -1)
		slot = tex_type + tex_slot_offset;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(std::get<3>(Texture::ParseFormat(tex_type)), tex_ID);
	//Tex_slot = slot;
}

void Texture::BindC(GLuint slot /*= -1*/, GLuint read_or_write /*= GL_READ_WRITE*/, GLuint _level /*= 0*/) const
{
	if (slot == -1)
		slot = tex_type + tex_slot_offset;

	auto [layout, _1, _2, _3] = Texture::ParseFormat(tex_type);

	GLuint is_array = _level == 0 ? GL_FALSE : GL_TRUE;

	glBindImageTexture(slot, tex_ID, 0, is_array, 0, read_or_write, layout);
}

void Texture::BindU(GLuint slot /*= -1*/) const
{
	if (slot == -1) slot = tex_type;

	glBindTextureUnit(slot, tex_ID);
}

void Texture::UnbindC(GLuint slot /*= -1*/, GLuint read_or_write /*= GL_READ_WRITE*/, GLuint _level /*= 0*/) const
{
	if (slot == -1)
		slot = tex_type + tex_slot_offset;

	auto [layout, _1, _2, _3] = Texture::ParseFormat(tex_type);

	GLuint is_array = _level == 0 ? GL_FALSE : GL_TRUE;

	glBindImageTexture(slot, 0, 0, is_array, 0, GL_READ_ONLY, layout);
}

void Texture::Unbind() const
{
	glBindTexture(std::get<3>(Texture::ParseFormat(tex_type)), 0);
	//glActiveTexture(0);
}

inline Texture::TexStorageInfo Texture::ParseFormat(TextureType _type)
{
	switch (_type)
	{
	case RGBA_TEXTURE:
	case BUFFER_TEXTURE:
	case SPIRIT_TEXURE:
		return { GL_RGBA8  , GL_RGBA, GL_UNSIGNED_BYTE, GL_TEXTURE_2D		};
	case RGB_TEXTURE:
		return { GL_RGB8   , GL_RGB,  GL_UNSIGNED_BYTE, GL_TEXTURE_2D		};
	case IBL_TEXTURE:
	case HDR_BUFFER_TEXTURE:
		return { GL_RGBA16F, GL_RGBA, GL_FLOAT,         GL_TEXTURE_2D		};
	case IBL_CUBE_TEXTURE:
		return { GL_RGBA16F, GL_RGBA, GL_FLOAT,         GL_TEXTURE_CUBE_MAP };
	case RG_TEXTURE:
	case FLOAT_BUFFER_TEXTURE:
		return { GL_RG16F,	 GL_RG,	  GL_FLOAT,			GL_TEXTURE_2D		};
	default:
		assert(false && "WRONG FORMAT");
		return { GL_NONE,	 GL_NONE ,GL_NONE,			GL_NONE				};
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

	if(_warp_r != 0)
		glTexParameteri(Type, GL_TEXTURE_WRAP_T, _warp_r);

	if (_lev_min + _lev_max != 0) {
		glTexParameteri(Type, GL_TEXTURE_BASE_LEVEL, _lev_min);
		glTexParameteri(Type, GL_TEXTURE_MAX_LEVEL, _lev_max);
	}
}

void Texture::GenIrradiaceConvFrom(const Texture& _Tar_Tex)
{
	GenIrradianceConv(_Tar_Tex.GetTexID(), _Tar_Tex.im_w, _Tar_Tex.im_h, _Tar_Tex.tex_type);
}

void Texture::GenIBLDiffuseFrom(const Texture& _Tar_Tex)
{
	GenIBLDiffuse(_Tar_Tex.GetTexID(), _Tar_Tex.im_w, _Tar_Tex.im_h, _Tar_Tex.tex_type);
}

void Texture::GenIBLSpecularFrom(const Texture& _Tar_Tex)
{
	GenIBLSpecular(_Tar_Tex.GetTexID(), _Tar_Tex.im_w, _Tar_Tex.im_h, _Tar_Tex.tex_type);
}

void Texture::GenCubeMapFrom(const Texture& _Tar_Tex, size_t res)
{
	GenCubeMap(_Tar_Tex.GetTexID(), res, _Tar_Tex.tex_type);
}

void Texture::GenIrradianceConv(GLuint _tar_ID, size_t _tar_w, size_t _tar_h, TextureType _tar_type /*= IBL_TEXTURE*/)
{
	assert(false && "this function has been abandoned");
}

void Texture::GenIBLSpecular(GLuint _tar_ID, size_t _tar_w, size_t _tar_h, TextureType _tar_type /*= IBL_TEXTURE*/)
{
	//////////////////   Specular Importance Sampling   //////////////////
	//			https://learnopengl.com/PBR/IBL/Specular-IBL			//

	const int max_inter = 8;

	GLuint ID;
	glGenTextures(1, &ID);		//for storage
	glBindTexture(GL_TEXTURE_2D, ID);

	Texture::SetTexParam<GL_TEXTURE_2D>(ID, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_MIRRORED_REPEAT, 0, max_inter - 1);

	auto [interlayout, layout, type, _] = Texture::ParseFormat(_tar_type);

	im_w = _tar_w; im_h = _tar_h; tex_type = _tar_type;

	LOOP(8) {
		int lod_w = im_w / pow(2, i);
		int lod_h = im_h / pow(2, i);

		lod_w -= lod_w % 4;
		lod_h -= lod_h % 4;

		glTexImage2D(GL_TEXTURE_2D, i, interlayout, lod_w, lod_h, 0, layout, type, NULL);
	}

	ComputeShader importance_samp = ComputeShader::ImportShader("Importance_Samp");
	LOOP(8) { // 0 1 2 3 4 5 6 7
		int lod_w = im_w / pow(2, i);
		int lod_h = im_h / pow(2, i);

		lod_w -= lod_w % 4;
		lod_h -= lod_h % 4;


		glBindImageTexture(_tar_type, _tar_ID, 0, GL_FALSE, 0, GL_READ_ONLY, interlayout);
		glBindImageTexture(_tar_type + 1, ID, i, GL_FALSE, 0, GL_WRITE_ONLY, interlayout); // IBL_TEXTURE -> IBL_TEXTURE + 1

		importance_samp.UseShader();
		importance_samp.SetValue("s", (float)i / (float)(max_inter - 1));
		importance_samp.SetValue("max_step", 32);
		importance_samp.RunComputeShader(lod_w / 4, lod_h / 4);
	}

	ResetTexID(ID);
}

void Texture::GenIBLDiffuse(GLuint _tar_ID, size_t _tar_w, size_t _tar_h, TextureType _tar_type /*= IBL_TEXTURE*/)
{
	//////////////////  Diffuse Irradience Convolution  //////////////////
	//       https://learnopengl.com/PBR/IBL/Diffuse-irradiance         //

	GLuint ID;
	glGenTextures(1, &ID);		//for storage
	glBindTexture(GL_TEXTURE_2D, ID);
	Texture::SetTexParam<GL_TEXTURE_2D>(ID, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_MIRRORED_REPEAT, 0, 1);

	im_w = _tar_w/8; im_h = _tar_h/8; tex_type = _tar_type;

	auto [interlayout, layout, type, _] = Texture::ParseFormat(_tar_type);
	glTexImage2D(GL_TEXTURE_2D, 0, interlayout, im_w, im_w, 0, layout, type, NULL);

	glBindImageTexture(_tar_type + 1, _tar_ID, 0, GL_FALSE, 0, GL_READ_ONLY, interlayout);
	glBindImageTexture(_tar_type, ID, 0, GL_FALSE, 0, GL_WRITE_ONLY, interlayout);

	ComputeShader irradiance_conv = ComputeShader::ImportShader("Irradiance_Conv");
	irradiance_conv.UseShader();
	irradiance_conv.SetValue("max_step", 32);
	irradiance_conv.RunComputeShader(im_w / 4, im_w / 4);

	ResetTexID(ID);
}

void Texture::GenCubeMap(GLuint _tar_ID, size_t _tar_res, TextureType _tar_type /*= IBL_TEXTURE*/)
{

	// https://learnopengl.com/Advanced-OpenGL/Cubemaps

	auto [interlayout, layout, type, _] = Texture::ParseFormat(_tar_type);

	GLuint ID;
	glGenTextures(1, &ID);		//for storage
	Texture::SetTexParam<GL_TEXTURE_CUBE_MAP>(ID, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0, 0, GL_CLAMP_TO_EDGE);

	DEBUG(std::get<3>(ParseFormat(tex_type))==GL_TEXTURE_CUBE_MAP)

	LOOP(6)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, interlayout, _tar_res, _tar_res, 0, layout, type, NULL);

	ComputeShader to_cubemap = ComputeShader::ImportShader("toCubeMap");

	glBindImageTexture(0, ID, 0, GL_TRUE, 0, GL_WRITE_ONLY, interlayout);
	glBindImageTexture(1, _tar_ID, 0, GL_FALSE, 0, GL_READ_ONLY, interlayout);

	to_cubemap.UseShader();
	to_cubemap.SetValue("resol", (int)_tar_res);
	to_cubemap.RunComputeShader(_tar_res / 4, _tar_res / 4, 6);

	ResetTexID(ID);

	tex_type = IBL_CUBE_TEXTURE;
	im_w = im_h = _tar_res;
}

std::unordered_map<std::string, std::shared_ptr<Texture>> TextureLib::t_tex_list{};

std::shared_ptr<Texture> TextureLib::GetTexture(const std::string& _name)
{
	if (t_tex_list.find(_name) == t_tex_list.end())
		return nullptr;

	return t_tex_list[_name];
}

GLuint TextureLib::GetTextureID(const std::string& _name)
{
	return TextureLib::GetTexture(_name)->GetTexID();
}

std::shared_ptr<Texture> TextureLib::Noise_2D_4x4()
{
	const std::string _name = "Uni.2D.4.4";
	auto result = GetTexture(_name);

	if (result != nullptr)
		return result;

	GenNoiseTexture(UNI_2D_NOISE, 4, 4);

	return t_tex_list[_name];
}

std::shared_ptr<Texture> TextureLib::Noise_2D_16x16()
{
	const std::string _name = "Uni.2D.16.16";
	auto result = GetTexture(_name);

	if (result != nullptr)
		return result;

	GenNoiseTexture(UNI_2D_NOISE, 16, 16);

	return t_tex_list[_name];
}

TextureLib::TextureRes TextureLib::IBL_LUT()
{
	const std::string _name = "IBL_LUT";
	auto result = GetTexture(_name);

	if (result != nullptr)
		return result;

	t_tex_list[_name] = std::make_shared<Texture>("ibl_brdf_lut.png", RGBA_TEXTURE, GL_CLAMP);

	return t_tex_list[_name];
}

void TextureLib::GenNoiseTexture(NoiseType _type, size_t _w, size_t _h)
{
	GLuint id;

	glGenTextures(1, &id);//for storage
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);

	Texture::SetTexParam<GL_TEXTURE_2D_ARRAY>(id, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, 0, 6);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA16F, _w, _h, 6);

	LOOP_N(6, level) {
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

	std::string noise_name = type_name + "." + std::to_string(dimension) + "D." + std::to_string(_w) + "." + std::to_string(_h);

	TextureLib::t_tex_list[noise_name] = std::make_shared<Texture>(_w, _h, id, IBL_TEXTURE, noise_name);
}
