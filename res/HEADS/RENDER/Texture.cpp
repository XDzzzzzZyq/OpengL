﻿#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& texpath, TextureType tex_type, GLuint Tile_type)
	:m_path(texpath), m_buffer(nullptr),Tex_type(tex_type), Tex_slot(tex_type),
	im_bpp(0), im_h(0), im_w(0)
{

	GLfloat maxAnti;
	//std::cout << Tex_ID << std::endl;
	stbi_set_flip_vertically_on_load(1);
	glGenTextures(1, &Tex_ID);
	glBindTexture(GL_TEXTURE_2D, Tex_ID);
	switch (tex_type)
	{
	case PNG_TEXTURE:

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
			std::cout << "Image texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
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
			std::cout << "Image texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
		}
		else {
			std::cout << "Image texture FAILED" << std::endl;
		}
		break;

	case HDR_TEXTURE:

		m_buffer_f = stbi_loadf(texpath.c_str(), &im_w, &im_h, &im_bpp, 4);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Tile_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Tile_type);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, im_w, im_h, 0, GL_RGBA, GL_FLOAT, m_buffer_f); //
		glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, im_w, im_h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, im_w, im_h, GL_RGBA, GL_FLOAT, m_buffer_f);
		
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

			std::cout << "HDR texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
		}else
			std::cout << "HDR texture FAILED"<< std::endl;
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, SCREEN_W, SCREEN_H, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);

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
	:m_path(""), m_buffer(nullptr), Tex_type(BUFFER_TEXTURE), Tex_slot(BUFFER_TEXTURE),
	im_bpp(8), im_h(y), im_w(x)
{
	glGenTextures(1, &Tex_ID);
	glBindTexture(GL_TEXTURE_2D, Tex_ID);

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
	glDeleteTextures(1, &Tex_ID);
}

void Texture::Resize(const ImVec2& size)
{
	im_h = size.y;
	im_w = size.x;
	glBindTexture(GL_TEXTURE_2D, Tex_ID);

	if(Tex_type == PNG_TEXTURE || Tex_type == BUFFER_TEXTURE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	else if(Tex_type == HDR_BUFFER_TEXTURE || Tex_type == HDR_TEXTURE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, im_w, im_h, 0, GL_RGBA, GL_FLOAT, NULL);
	else 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, im_w, im_h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::Resize(float x, float y)
{
	im_h = y;
	im_w = x;
	glBindTexture(GL_TEXTURE_2D, Tex_ID);
	
	if(Tex_type == BUFFER_TEXTURE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	else if (Tex_type == HDR_BUFFER_TEXTURE)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, im_w, im_h, 0, GL_RGBA, GL_FLOAT, NULL);
	else 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, im_w, im_h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::Bind(GLuint slot) const
{
	if (slot == -1)
		slot = Tex_slot;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, Tex_ID);
	//Tex_slot = slot;
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(0);
}
