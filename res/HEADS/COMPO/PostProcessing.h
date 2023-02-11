#pragma once

#include <optional>
#include <variant>

#include "Field.h"

#include "Shaders.h"
#include "Texture.h"
#include "BUFFER/FrameBuffer.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class PostProcessing
{
public:
	Field pps_field;

public:
	PostProcessing(const std::string& _shader, ShaderType _type = FRAGMENT_SHADER);

private:
	VertexArray o_vertArry;
	VertexBuffer o_vertBuffer;
	IndexBuffer o_indexBuffer;

public:
	std::variant<RenderShader, ComputeShader> pps_shader;
	FrameBuffer pps_fb;
	template<typename T>
	void SetShaderValue(std::string _name, T _v);

public:
	void RenderPPS();

};

template<typename T>
void PostProcessing::SetShaderValue(std::string _name, T _v)
{
	std::get<RenderShader>(pps_shader).UseShader();
	std::get<RenderShader>(pps_shader).SetValue(_name, _v);
	std::get<RenderShader>(pps_shader).UnuseShader();
}

