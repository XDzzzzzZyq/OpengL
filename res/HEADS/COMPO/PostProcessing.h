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
	Sprite pps_sprite;

public:
	PostProcessing(const std::string& _shader, ShaderType _type = FRAGMENT_SHADER);

private:

	std::unordered_map<std::string, GLuint> pps_bindings;
	void UpdateBindings();

public:
	std::optional<RenderShader> pps_shader;
	FrameBuffer pps_fb;
	template<typename... T>
	void SetShaderValue(const std::string& _name, T ..._v);
	void SetShaderValue(const std::string& _name, GLsizei _count, const float* va0, ArrayType _TYPE);
	void AddBinding(std::string _pass_name, GLuint _slot);

	void* GetShaderStruct() override { return dynamic_cast<ShaderLib*>( &pps_shader.value() ); }
	void* GetTransform()	override { return dynamic_cast<Transform*>( pps_field.GetTransformPtr()); }

public:
	void RenderPPS();
	void RenderPPSSpr(Camera* cam);
};

template<typename... T>
void PostProcessing::SetShaderValue(const std::string& _name, T ..._v)
{
	pps_shader->UseShader();
	pps_shader->SetValue(_name, _v...);
	pps_shader->UnuseShader();
}

