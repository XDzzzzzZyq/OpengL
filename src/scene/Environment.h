#pragma once

#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "Sprite.h"
#include "Camera.h"
#include "MeshData.h"

#include "buffer/FrameBuffer.h"

#include "Shaders.h"

enum EnvironmentType
{
	NONE_ENVIR, TEXTURE_ENVIR, COLOR_ENVIRN, NOISE_ENVIRN
};

class Environment : public GameObject, public Transform3D
{
public:

	Environment();
	Environment(const std::string& texpath);

public:

	RenderShader envir_shader;

	Texture envir_IBL_diff, envir_IBL_spec;
	EnvironmentType envir_type = EnvironmentType::NONE_ENVIR;
	bool use_envir = false;

	Sprite envir_sprite;

	float envir_gamma = 2.2f;
	glm::vec3 envir_color = glm::vec3(1.0f);

public:

	void ChangeEnvirTexture(const std::string& texpath) const;
	void ChangeEnvirType(EnvironmentType type) const;

	void BindFrameBuffer() const;
	void UnbindFrameBuffer() const;
	void SwapFrameBuffer(FBType type);

	void BindEnvironTexture() const;
	void UnbindEnvironTexture() const;

public:

	void* GetShader()		override { return &envir_shader; }
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }

	void RenderEnvironment(Camera* cam);
	void RenderEnvirSpr(Camera* cam);

};

