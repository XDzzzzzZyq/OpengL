#pragma once

#include <optional>
#include <variant>
#include <unordered_map>

#include "Field.h"
#include "GameObject.h"

#include "Shaders.h"
#include "Texture.h"
#include "BUFFER/FrameBuffer.h"

#include "Sprite.h"

class PostProcessing : public GameObject
{
public:
	Field pps_field{};
	Sprite pps_sprite{};
	ShaderType pps_type{};

public:
	PostProcessing(const std::string& _shader, ShaderType _type = FRAGMENT_SHADER);

private:

	FrameBuffer pps_fb;
	std::unordered_map<std::string, GLuint> pps_bindings;
	void UpdateBindings();

public:

	std::shared_ptr<Shaders> pps_shader;

	template<typename... T>
	void SetShaderValue(const std::string& _name, T ..._v);
	void SetShaderValue(const std::string& _name, GLsizei _count, const float* va0, ArrayType _TYPE);
	void AddBinding(std::string _pass_name, GLuint _slot);

	void* GetShader()		override { return pps_shader.get(); };
	void* GetTransform()	override { return dynamic_cast<Transform*>( pps_field.GetTransformPtr()); }

public:
	void RenderPPS(const glm::vec2& _scr_size = glm::vec2(0), GLuint _batch = 16);
	void RenderPPSSpr(Camera* cam);
};

template<typename... T>
void PostProcessing::SetShaderValue(const std::string& _name, T ..._v)
{
	pps_shader->UseShader();
	pps_shader->SetValue(_name, _v...);
}

