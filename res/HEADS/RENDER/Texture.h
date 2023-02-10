#pragma once
#include <iostream>
#include <string>
#include <GL/glew.h>

#include "support.h"

#include "Shaders.h"

enum TextureType
{
	NONE_TEXTURE, RGBA_TEXTURE, RGB_TEXTURE, SPIRIT_TEXURE, IBL_TEXTURE, BUFFER_TEXTURE = 6, HDR_BUFFER_TEXTURE, FLOAT_BUFFER_TEXTURE
};

class Texture
{
private:
	int im_w = 0, im_h = 0, im_bpp = 0;
	std::string m_path;
	GLubyte* m_buffer;
	GLfloat* m_buffer_f;
	GLuint Tex_ID = 0;
	
public:
	TextureType Tex_type = TextureType::NONE_TEXTURE;
	mutable GLuint Tex_slot;

	Texture(const std::string& texpath, TextureType tex_type, GLuint Tile_type); //using tex_type as its slot, but you can change it if you want
	Texture(GLuint Tile_type, int x, int y);    //for FBO
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

public: // for texture processing
	void GenIrradiaceConvFrom(GLuint _Tar_ID);
	void GenIrradiaceConvFrom(const Texture& _Tar_Tex);
private:
	void GenIrradianceConv(GLuint _tar_ID, int _tar_w, int _tar_h, TextureType _tar_type = IBL_TEXTURE);
};

