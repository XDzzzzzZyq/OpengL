#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "GameObject.h"
#include "MeshData.h"

#include "Texture.h"
#include "Shaders.h"
#include "Camera.h"

#include "support.h"

#include <optional>

#define SPIRIT_SIZE 0.7f

enum SpiritType
{
	NONE_SPIRIT, POINT_LIGHT_SPIRIT, SUN_LIGHT_SPIRIT, SPOT_LIGHT_SPIRIT, CAM_SPIRIT, ENVIRN_SPIRIT, PARTIC_SPIRIT
};

class Spirit : public GameObject
{

private:
	static std::string fileroot;
public:

	std::optional<Texture> r_tex;

public:
	float spirit_opacity = 0.9f;

	SpiritType spr_type = SpiritType::NONE_SPIRIT;
	mutable std::optional<RenderShader> r_shader;

	Spirit();
	~Spirit();

	void RenderSpirit(const glm::vec3& pos, const glm::vec3& col, Camera* cam);
	void RenderSpirit(Camera* cam);

	void SetSpiritShader();
	std::string ParsePath() const;
	void SetTex();
	ShaderLib* GetShaderStruct() override { return dynamic_cast<ShaderLib*>(&r_shader.value()); };

	void DeleteSpirit();
};

