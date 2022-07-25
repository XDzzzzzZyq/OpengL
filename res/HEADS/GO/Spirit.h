#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "GameObject.h"

#include "Texture.h"
#include "Shaders.h"

#include "support.h"

#define SPIRIT_SIZE 0.7f

enum SpiritType
{
	NONESPIRIT,LIGHTSPIRIT,CAMSPIRIT,ENVIRNSPIRIT
};

class Spirit : public GameObject
{

private:
	VertexArray r_vertArry;
	VertexBuffer r_vertBuffer;
	IndexBuffer r_index;
	Texture r_tex;

	float spirit_opacity = 0.8f;

	std::vector<float> VertData = {
		-1.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		-1.0f,-1.0f, 0.0f,		0.0f, 1.0f,
		 1.0f,-1.0f, 0.0f,		1.0f, 1.0f
	};
public:

	SpiritType spr_type = SpiritType::NONESPIRIT;
	mutable Shaders r_shader;

	Spirit();
	~Spirit();



	void RenderSpirit(const std::vector<float>& light_pos, const glm::mat4& cam_Trans, const glm::mat4& cam_projec);

	void SetSpiritShader();
	std::string ParsePath() const;
	void SetTex();

	void DeleteSpirit();
};

