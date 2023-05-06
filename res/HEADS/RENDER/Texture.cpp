#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& texpath, TextureType tex_type, GLuint Tile_type)
	:m_path(texpath), tex_type(tex_type),
	im_bpp(0), im_h(0), im_w(0)
{

	GLfloat maxAnti;
	//std::cout << Tex_ID << std::endl;
	stbi_set_flip_vertically_on_load(1);
	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	GLubyte* m_buffer = nullptr;
	GLfloat* m_buffer_f = nullptr;

	switch (tex_type)
	{
	case RGBA_TEXTURE:

		m_buffer = stbi_load(texpath.c_str(), &im_w, &im_h, &im_bpp, 4);

		// std::cout << Tex_ID << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Tile_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Tile_type);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);

#if 1
		glGenerateMipmap(GL_TEXTURE_2D);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
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
		m_buffer = stbi_load(texpath.c_str(), &im_w, &im_h, &im_bpp, 4);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Tile_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Tile_type);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);

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
		m_buffer_f = stbi_loadf(texpath.c_str(), &im_w, &im_h, &im_bpp, 4);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Tile_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Tile_type);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, im_w, im_h, 0, GL_RGBA, GL_FLOAT, m_buffer_f); //
		glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA16F, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, GL_RGBA, GL_FLOAT, m_buffer_f);

#if 1
		glGenerateMipmap(GL_TEXTURE_2D);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);
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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_W, SCREEN_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);
		break;

	case HDR_BUFFER_TEXTURE:

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_W, SCREEN_H, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);
		break;

	case FLOAT_BUFFER_TEXTURE:

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, SCREEN_W, SCREEN_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

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
	:m_path(""), tex_type(BUFFER_TEXTURE),
	im_bpp(8), im_h(y), im_w(x)
{
	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
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

	if (tex_type == BUFFER_TEXTURE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	else if (tex_type == HDR_BUFFER_TEXTURE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, im_w, im_h, 0, GL_RGBA, GL_FLOAT, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::Bind(GLuint slot) const
{
	if (slot == -1)
		slot = tex_type + tex_slot_offset;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex_ID);
	//Tex_slot = slot;
}

void Texture::BindC(GLuint slot /*= -1*/, GLuint read_or_write /*= GL_READ_WRITE*/) const
{
	if (slot == -1)
		slot = tex_type + tex_slot_offset;

	GLuint layout = GL_NONE;

	if		(tex_type == RGB_TEXTURE) layout = GL_RGB8;
	else if (tex_type == RGBA_TEXTURE || tex_type == BUFFER_TEXTURE) layout = GL_RGBA8;
	else if (tex_type == IBL_TEXTURE || tex_type == HDR_BUFFER_TEXTURE) layout == GL_RGBA16F;

	glBindImageTexture(slot, tex_ID, 0, GL_FALSE, 0, read_or_write, layout);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(0);
}

void Texture::GenIrradiaceConvFrom(GLuint _Tar_ID)
{
	int w, h;
	glBindTexture(GL_TEXTURE_2D, _Tar_ID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);// DEBUG(w)
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);// DEBUG(h)

	if (_Tar_ID == tex_ID)
		DEBUG("are you sure about that?");

	GenIrradianceConv(_Tar_ID, w, h);
}

void Texture::GenIrradiaceConvFrom(const Texture& _Tar_Tex)
{
	GenIrradianceConv(_Tar_Tex.GetTexID(), _Tar_Tex.im_w, _Tar_Tex.im_h, _Tar_Tex.tex_type);
}

void Texture::GenIrradianceConv(GLuint _tar_ID, int _tar_w, int _tar_h, TextureType _tar_type /*= IBL_TEXTURE*/)
{
	Timer timer("Irradiance Convolution");
	
	if (tex_ID != 0)DelTexture();   //reset

	glGenTextures(1, &tex_ID);//for storage
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);


	im_w = _tar_w/2; im_h = _tar_h/2; tex_type = _tar_type;


	switch (_tar_type)
	{
	case RGBA_TEXTURE:
		glTexStorage2D(GL_TEXTURE_2D, 6, GL_RGBA8, im_w, im_h);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		break;
	case RGB_TEXTURE:
		glTexStorage2D(GL_TEXTURE_2D, 6, GL_RGB8, im_w, im_h);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		break;
	case IBL_TEXTURE:
		glTexStorage2D(GL_TEXTURE_2D, 6, GL_RGBA16F, im_w, im_h);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, lod_w, lod_h, GL_RGBA, GL_FLOAT, nullptr);
		break;
	default:
		break;
	}

	GLDEBUG
	LOOP(6) { //0 1 2 3 4 5
		int lod_w = im_w / pow(2, i);
		int lod_h = im_h / pow(2, i);

		static ComputeShader irradiance_conv = ComputeShader("Irradiance_Conv");

		//glActiveTexture(GL_TEXTURE0 + _tar_type + 1);
		glBindImageTexture(_tar_type, _tar_ID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
		glBindImageTexture(this->tex_type + 1, this->tex_ID, i, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F); // IBL_TEXTURE -> IBL_TEXTURE + 1

		irradiance_conv.UseShader();;
		irradiance_conv.SetValue("s", (float)(i+1)/6);
		irradiance_conv.SetValue("max_step", 32 / (int)pow(2, 5-i));
		irradiance_conv.RunComputeShader(lod_w, lod_h);

	}
	
}
