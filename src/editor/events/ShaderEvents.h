#pragma once

#include "Shaders.h"
#include <string>

// Shader Code Editing

struct ShaderCodeCompileEvent
{
	Shaders* shader;
	ShaderType type;
	std::string code;
};

// Shader Struct Editing

struct ShaderStructAddArrayBufferEvent
{
	Shaders* shader;
	ShaderType type;
	S_AB layout;
};

struct ShaderStructAddPassEvent
{
	Shaders* shader;
	ShaderType type;
	S_REND layout;
};

struct ShaderStructAddInputEvent
{
	Shaders* shader;
	ShaderType type;
	S_IN input;
};

struct ShaderStructAddOutputEvent
{
	Shaders* shader;
	ShaderType type;
	S_OUT output;
};

struct ShaderStructAddUniformEvent
{
	Shaders* shader;
	ShaderType type;
	S_U uniform;
};

struct ShaderStructAddStructEvent
{
	Shaders* shader;
	ShaderType type;
	std::string name;
	Args args;
};

struct ShaderStructAddGlobEvent
{
	Shaders* shader;
	ShaderType type;
	S_glob glob;
};

struct ShaderStructAddConstEvent
{
	Shaders* shader;
	ShaderType type;
	S_const con;
};

struct ShaderStructAddFuncEvent
{
	Shaders* shader;
	ShaderType type;
	S_func func;
};

struct ShaderStructCompileEvent
{
	Shaders* shader;
	ShaderType type;
};

// Shader Node Editing

struct ShaderNodeCompileEvent
{
	Shaders* shader;
	ShaderType type;
};

// Saving

struct ShaderSaveEvent
{
	Shaders* shader;
	ShaderType type;
};

// Shader changed event for Editor update
struct ShaderChangedEvent {
	Shaders* shader;
	ShaderType type;
};
