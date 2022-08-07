#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& texpath, TextureType tex_type, GLuint Tile_type)
	:m_path(texpath), m_buffer(nullptr),Tex_type(tex_type), Tex_slot(tex_type),
	im_bpp(0), im_h(0), im_w(0)
{
	//std::cout << Tex_ID << std::endl;
	stbi_set_flip_vertically_on_load(1);

	switch (tex_type)
	{
	case PNG_TEXTURE:

		m_buffer = stbi_load(texpath.c_str(), &im_w, &im_h, &im_bpp, 4);

		glGenTextures(1, &Tex_ID);
		glBindTexture(GL_TEXTURE_2D, Tex_ID);
		// std::cout << Tex_ID << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Tile_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Tile_type);


		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);

		glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "Image texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
		glBindTexture(GL_TEXTURE_2D, 0);
		//std::cout << im_bpp << std::endl;
		if (m_buffer) {
			stbi_image_free(m_buffer);
		}
		break;
	case HDR_TEXTURE:

		/*
		m_buffer_f = stbi_loadf(texpath.c_str(), &im_w, &im_h, &im_bpp, 4);

		glGenTextures(1, &Tex_ID);
		glBindTexture(GL_TEXTURE_2D, Tex_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, im_w, im_h, 0, GL_RGB, GL_FLOAT, m_buffer_f);
		std::cout << "HDR texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
		glBindTexture(GL_TEXTURE_2D, 0);
		//std::cout << im_bpp << std::endl;
		if (m_buffer_f) {
			stbi_image_free(m_buffer_f);
		}*/

		m_buffer = stbi_load(texpath.c_str(), &im_w, &im_h, &im_bpp, 4);

		glGenTextures(1, &Tex_ID);
		glBindTexture(GL_TEXTURE_2D, Tex_ID);
		// std::cout << Tex_ID << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Tile_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Tile_type);


		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);

		glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "HDR texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
		glBindTexture(GL_TEXTURE_2D, 0);
		//std::cout << im_bpp << std::endl;
		if (m_buffer) {
			stbi_image_free(m_buffer);
		}

		break;
	case BUFFER_TEXTURE:

		glGenTextures(1, &Tex_ID);
		glBindTexture(GL_TEXTURE_2D, Tex_ID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_W, SCREEN_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::Resize(float x, float y)
{
	im_h = y;
	im_w = x;
	glBindTexture(GL_TEXTURE_2D, Tex_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::Bind(GLuint slot /*= 0*/) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, Tex_ID);
	//Tex_slot = slot;
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(0);
}
