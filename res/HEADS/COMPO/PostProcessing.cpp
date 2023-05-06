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
		pps_shader = ComputeShader(_shader_name);
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
}

void PostProcessing::SetShaderValue(const std::string& _name, GLsizei _count, const float* va0, ArrayType _TYPE)
{
	std::get<RenderShader>(pps_shader).UseShader();
	std::get<RenderShader>(pps_shader).SetValue(_name, _count, va0, _TYPE);
	std::get<RenderShader>(pps_shader).UnuseShader();
}

void PostProcessing::RenderPPS()
{
	pps_vertArry.Bind();
	std::get<RenderShader>(pps_shader).UseShader();
	pps_indexBuffer.Bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
