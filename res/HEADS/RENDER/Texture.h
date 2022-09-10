#pragma once
#include <iostream>
#include <string>
#include <GL/glew.h>

#include "support.h"

enum TextureType
{
	NONE_TEXTURE, PNG_TEXTURE, JPG_TEXTURE, HDR_TEXTURE, BUFFER_TEXTURE, HDR_BUFFER_TEXTURE, FLOAT_BUFFER_TEXTURE
};

class Texture
{
private:
	int im_w, im_h, im_bpp = 0;
	std::string m_path;
	GLubyte* m_buffer;
	GLfloat* m_buffer_f;
	GLuint Tex_ID;
	
public:
	TextureType Tex_type = TextureType::NONE_TEXTURE;
	mutable GLuint Tex_slot;

	Texture(const std::string& texpath, TextureType tex_type,GLuint Tile_type);
	Texture(GLuint Tile_type, int x, int y);    //for fbo
	Texture();
	~Texture();

	void DelTexture() const;
	void SlotAdd(int delt) { Tex_slot += delt; }
	void Resize(const ImVec2& size);
	void Resize(float x, float y);

	void Bind(GLuint slot = -1) const;
	void Unbind() const;

	inline int GetW()const { return im_w; }
	inline int GetH()const { return im_h; }
	inline int GetDPP()const { return im_bpp; }
	GLuint GetTexID() const { return Tex_ID; }
};

