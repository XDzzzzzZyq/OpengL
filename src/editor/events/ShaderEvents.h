#pragma once

#include "Shaders.h"
#include <string>

struct ShaderCodeCompileEvent
{
	Shaders* shader;
	ShaderType type;
	std::string code;
};

struct ShaderStructCompileEvent
{
	Shaders* shader;
	ShaderType type;
};

struct ShaderNodeCompileEvent
{
	Shaders* shader;
	ShaderType type;
};

struct ShaderSaveEvent
{
	Shaders* shader;
	ShaderType type;
};