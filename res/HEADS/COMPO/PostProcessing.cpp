#include "PostProcessing.h"

PostProcessing::PostProcessing(const std::string& _shader_name, ShaderType _type)
{

	static std::vector<float> screenQuad = {
		// positions		// texCoords
		-1.0f, 1.0f,	0.0f, 1.0f,
		 1.0f, 1.0f,	1.0f, 1.0f,
		-1.0f,-1.0f,	0.0f, 0.0f,
		 1.0f,-1.0f,	1.0f, 0.0f

	};

	static auto indexArray = std::vector<GLuint>{ 0,2,1,1,2,3 };

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

	pps_vertBuffer = VertexBuffer(screenQuad.data(), screenQuad.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(2); //2D position
	layout.Push<float>(2); //UV

	pps_vertArry.AddBuffer(pps_vertBuffer, layout);

	GLuint* index = indexArray.data();

	pps_indexBuffer = IndexBuffer(index, indexArray.size() * sizeof(GLuint));

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
	pps_vertArry.Bind();
	pps_shader->UseShader();
	pps_indexBuffer.Bind();

	if (pps_shader->is_shader_changed)
		UpdateBindings();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void PostProcessing::RenderPPSSpr(Camera* cam)
{
	pps_sprite.RenderSprite(pps_field.o_position, glm::vec3(1), cam);
}
