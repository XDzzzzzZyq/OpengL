#include "PostProcessing.h"

PostProcessing::PostProcessing(const std::string& _shader_name, ShaderType _type)
{
	switch (_type)
	{
	case FRAGMENT_SHADER:
		pps_shader = RenderShader("Screen", _shader_name);
		break;
	case COMPUTE_SHADER:
		pps_shader = RenderShader("Screen", _shader_name);
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
	pps_shader->UnuseShader();
}

void PostProcessing::AddBinding(std::string _pass_name, GLuint _slot)
{
	pps_bindings[_pass_name] = _slot;
}

void PostProcessing::RenderPPS()
{
	pps_shader->UseShader();

	if (pps_shader->is_shader_changed)
		UpdateBindings();

	MeshLib::Square->RenderObjProxy();
}

void PostProcessing::RenderPPSSpr(Camera* cam)
{
	pps_sprite.RenderSprite(pps_field.o_position, glm::vec3(1), cam);
}
