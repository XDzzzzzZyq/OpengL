#include "PostProcessing.h"

PostProcessing::PostProcessing(const std::string& _shader_name, ShaderType _type)
	:pps_type(_type)
{
	switch (_type)
	{
	case FRAGMENT_SHADER:
		pps_shader = std::dynamic_pointer_cast<Shaders>(std::make_shared<RenderShader>("Screen", _shader_name));
		break;
	case COMPUTE_SHADER:
		pps_shader = std::dynamic_pointer_cast<Shaders>(ComputeShader::ImportShaderSrc(_shader_name));
		break;
	default:
		break;
	}

	o_name = _shader_name;

	pps_sprite.spr_type = ENVIRN_SPRITE;
	pps_sprite.SetTex();
}

void PostProcessing::UpdateBindings()
{
	pps_shader->UseShader();
	for (auto& binds : pps_bindings)
		pps_shader->SetValue(binds.first, (int)binds.second);
}

void PostProcessing::SetShaderValue(const std::string& _name, GLsizei _count, const float* va0, ArrayType _TYPE)
{
	pps_shader->UseShader();
	pps_shader->SetValue(_name, _count, va0, _TYPE);
}

void PostProcessing::AddBinding(std::string _pass_name, GLuint _slot)
{
	pps_bindings[_pass_name] = _slot;
}

void PostProcessing::RenderPPS(const glm::vec2& _scr_size /*= glm::vec2(0)*/, GLuint _batch /*= 16*/)
{
	if (pps_shader->is_shader_changed)
		UpdateBindings();
	
	pps_shader->UseShader();

	switch (pps_type)
	{
	case FRAGMENT_SHADER:
		MeshLib::Square->RenderObjProxy();
		break;
	case COMPUTE_SHADER:
		dynamic_pointer_cast<ComputeShader>(pps_shader)->RunComputeShaderSCR(_scr_size, _batch, true);
		break;
	default:
		break;
	}
}

void PostProcessing::RenderPPSSpr(Camera* cam)
{
	pps_sprite.RenderSprite(pps_field.o_position, glm::vec3(1), cam);
}
