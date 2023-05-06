#pragma once

#include <optional>
#include <variant>

#include "Field.h"
#include "GameObject.h"

#include "Shaders.h"
#include "Texture.h"
#include "BUFFER/FrameBuffer.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class PostProcessing : public GameObject
{
public:
	Field pps_field;

public:
	PostProcessing(const std::string& _shader, ShaderType _type = FRAGMENT_SHADER);

private:
	VertexArray pps_vertArry;
	VertexBuffer pps_vertBuffer;
	IndexBuffer pps_indexBuffer;

public:
	std::variant<RenderShader, ComputeShader> pps_shader;
	FrameBuffer pps_fb;
	template<typename T>
	void SetShaderValue(const std::string& _name, T _v);
	void SetShaderValue(const std::string& _name, GLsizei _count, const float* va0, ArrayType _TYPE);

public:
	void RenderPPS();

};

template<typename T>
void PostProcessing::SetShaderValue(const std::string& _name, T _v)
{
	std::get<RenderShader>(pps_shader).UseShader();
	std::get<RenderShader>(pps_shader).SetValue(_name, _v);
	std::get<RenderShader>(pps_shader).UnuseShader();
}

