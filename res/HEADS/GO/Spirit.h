#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "GameObject.h"

#include "Texture.h"
#include "Shaders.h"
#include "Camera.h"

#include "support.h"

#define SPIRIT_SIZE 0.7f

enum SpiritType
{
	NONE_SPIRIT, LIGHT_SPIRIT, CAM_SPIRIT, ENVIRN_SPIRIT, PARTIC_SPIRIT
};

class Spirit : public GameObject
{

private:
	VertexArray r_vertArry;
	VertexBuffer r_vertBuffer;
	IndexBuffer r_index;
	Texture r_tex;

	std::vector<float> VertData = {
		-1.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		-1.0f,-1.0f, 0.0f,		0.0f, 1.0f,
		 1.0f,-1.0f, 0.0f,		1.0f, 1.0f
	};
public:
	float spirit_opacity = 0.8f;

	SpiritType spr_type = SpiritType::NONE_SPIRIT;
	mutable Shaders r_shader;

	Spirit();
	~Spirit();



	void RenderSpirit(const std::vector<float>& light_pos, Camera* cam);

	void SetSpiritShader();
	std::string ParsePath() const;
	void SetTex();

	void DeleteSpirit();
};

