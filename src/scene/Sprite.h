#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "GameObject.h"
#include "MeshData.h"

#include "Texture.h"
#include "Shaders.h"
#include "Camera.h"

#define SPRITE_SIZE 0.7f

enum SpriteType
{
	NONE_SPRITE, POINT_LIGHT_SPRITE, SUN_LIGHT_SPRITE, SPOT_LIGHT_SPRITE, CAM_SPRITE, ENVIRN_SPRITE, PARTIC_SPIRIT
};

class Sprite : public GameObject
{

private:
	static std::string fileroot;
public:

	Texture spr_tex;
	float spr_opacity = 0.9f;

	SpriteType spr_type = SpriteType::NONE_SPRITE;
	RenderShader spr_shader;

	Sprite();

	void RenderSprite(const glm::vec3& pos, const glm::vec3& col, Camera* cam);
	void RenderSprite(Camera* cam);

	void SetSpriteShader();
	std::string ParsePath() const;
	void SetTex();
	void* GetShader() override { return &spr_shader; };
};

