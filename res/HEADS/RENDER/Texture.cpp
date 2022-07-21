#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& texpath, GLuint Tile_type)
	:m_path(texpath), m_buffer(nullptr),
	im_bpp(0), im_h(0), im_w(0)
{
	//std::cout << Tex_ID << std::endl;
	stbi_set_flip_vertically_on_load(1);
	m_buffer = stbi_load(texpath.c_str(), &im_w, &im_h, &im_bpp, 4);

	glGenTextures(1, &Tex_ID);
	glBindTexture(GL_TEXTURE_2D, Tex_ID);
	// std::cout << Tex_ID << std::endl;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Tile_type);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Tile_type);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, im_w, im_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
	std::cout << "texture has been load successfully! [" << im_w << ":" << im_h << "]" << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);
	//std::cout << im_bpp << std::endl;
	if (m_buffer) {
		stbi_image_free(m_buffer);
	}
}

Texture::Texture()
{

}

Texture::~Texture()
{

}

void Texture::DelTexture() const
{
	glDeleteTextures(1, &Tex_ID);
}

void Texture::Bind(GLuint slot /*= 0*/) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, Tex_ID);
	Tex_slot = slot;
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
