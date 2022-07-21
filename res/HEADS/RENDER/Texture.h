#pragma once
#include <iostream>
#include <string>
#include <GL/glew.h>

enum TextureType
{
	NONE_TEXTURE, IMAGE_TEXTURE, HDR_TEXTURE
};

class Texture
{
private:
	int im_w, im_h, im_bpp = 0;
	std::string m_path;
	GLubyte* m_buffer;
	GLuint Tex_ID;
	
public:
	TextureType texType = TextureType::NONE_TEXTURE;
	mutable GLuint Tex_slot;

	Texture(const std::string& texpath, GLuint Tile_type);
	Texture();
	~Texture();

	void DelTexture() const;

	void Bind(GLuint slot = 0) const;
	void Unbind();

	inline int GetW()const { return im_w; }
	inline int GetH()const { return im_h; }
	inline int GetDPP()const { return im_bpp; }
};

