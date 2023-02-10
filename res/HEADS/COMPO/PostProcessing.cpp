#include "PostProcessing.h"

PostProcessing::PostProcessing(const std::string& _shader_name, ShaderType _type)
{
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

	o_vertBuffer = VertexBuffer(screenQuad.data(), screenQuad.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(2); //2D position
	layout.Push<float>(2); //UV

	o_vertArry.AddBuffer(o_vertBuffer, layout);

	auto* indexArray = new std::vector<GLuint>{ 0,2,1,1,2,3 };
	GLuint* index = indexArray->data();

	o_indexBuffer = IndexBuffer(index, indexArray->size() * sizeof(GLuint));

	delete indexArray;
}

void PostProcessing::RenderPPS()
{
	o_vertArry.Bind();
	std::get<RenderShader>(pps_shader).UseShader();
	o_indexBuffer.Bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
