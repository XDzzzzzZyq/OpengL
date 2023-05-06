#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "Spirit.h"
#include "Camera.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"

#include "support.h"
#include "structs.h"
#include "Shaders.h"

#include "glm/glm.hpp"

#include <optional>

enum EnvironmentType
{
	NONE_ENVIR, TEXTURE_ENVIR, COLOR_ENVIRN, NOISE_ENVIRN
};

class Environment : public GameObject, public Transform3D
{
private:
	VertexArray o_vertArry;
	VertexBuffer o_vertBuffer;
	IndexBuffer o_indexBuffer;

public:
	// std::optional<FrameBuffer> envir_frameBuffer;
	std::optional<RenderShader> envir_shader;

	Texture envir_IBL_spec, envir_IBL_diff;
	EnvironmentType envir_type = EnvironmentType::NONE_ENVIR;
	bool use_envir = false;

	Spirit envir_spirit;

	float envir_gamma = 2.2f;
	glm::vec3 envir_color = glm::vec3(0.1f);

	Environment(const std::string& texpath);
	Environment();
	~Environment();

	void ChangeEnvirTexture(const std::string& texpath) const;
	void ChangeEnvirType(EnvironmentType type) const;

	void BindFrameBuffer() const;
	void UnbindFrameBuffer() const;
	void SwapFrameBuffer(FBType type);

	void BindEnvironTexture() const;
	void UnbindEnvironTexture() const;

	mutable std::vector<float> envir_floatData;
	void GenFloatData() const;
	ShaderLib* GetShaderStruct() { return dynamic_cast<ShaderLib*>(&envir_shader.value()); }

	void RenderEnvironment(Camera* cam);


};

