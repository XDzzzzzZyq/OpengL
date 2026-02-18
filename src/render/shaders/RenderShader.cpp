#include "RenderShader.h"
#include "operator.h"
#include "structs.h"
#include "macros.h"
#include "ShaderLib.h"

void RenderShader::CreatShader(const std::string& verShader, const std::string& fragShader) {
	program_id = glCreateProgram();

	shader_data[VERTEX_SHADER].sh_ID = CompileShaderCode(VERTEX_SHADER, verShader);
	shader_data[FRAGMENT_SHADER].sh_ID = CompileShaderCode(FRAGMENT_SHADER, fragShader);

	glAttachShader(program_id, shader_data[VERTEX_SHADER].sh_ID);
	glAttachShader(program_id, shader_data[FRAGMENT_SHADER].sh_ID);

	glLinkProgram(program_id);
	glValidateProgram(program_id);

	//glDeleteShader(vs_id);
	//glDeleteShader(fs_id);
	//glDeleteProgram(program_id);
}

////////////////////////////////////////////////////////
RenderShader::RenderShader(const std::string& vert, const std::string& frag)
{
	shader_data[VERTEX_SHADER].sh_name = vert;
	shader_data[FRAGMENT_SHADER].sh_name = frag == "" ? vert : frag;

	shader_data[VERTEX_SHADER].sh_struct = ShaderStruct();
	shader_data[FRAGMENT_SHADER].sh_struct = ShaderStruct();

	shader_data[VERTEX_SHADER].sh_type = VERTEX_SHADER;
	shader_data[FRAGMENT_SHADER].sh_type = FRAGMENT_SHADER;

	ParseShaderFile(shader_data[VERTEX_SHADER].sh_name, VERTEX_SHADER);
	ParseShaderFile(shader_data[FRAGMENT_SHADER].sh_name, FRAGMENT_SHADER);
	
	GenerateShader(VERTEX_SHADER);
	GenerateShader(FRAGMENT_SHADER);

	CreatShader(shader_data[VERTEX_SHADER].sh_code, shader_data[FRAGMENT_SHADER].sh_code);
}

RenderShader::RenderShader()
{
}

RenderShader::~RenderShader()
{
}

void RenderShader::ParseShaderFile(std::string _name, ShaderType _type) {
	Timer timer("ParseShader");

	shader_data[_type].sh_struct->is_struct_changed = false;

	if (_name.find(Shaders::folder_root) == std::string::npos)
		_name = Shaders::folder_root + _name + Shaders::file_type[_type];

	std::ifstream Stream(_name);

	ParseShaderStream(Stream, _type);


	std::cout << "shaders are loaded up successfully!" << std::endl;
	//m_shaders = { shaders[0].str(),shaders[1].str() };
}

void RenderShader::ParseShaderCode(const std::string& _code, ShaderType _type)
{
	ShaderUnit* shader = GetShaderUnit(_type);
	shader->sh_struct->Reset();
	_LINK_LOC = {};
	if (!_code.empty())
		shader->sh_code = _code;

	std::stringstream Stream(shader->sh_code);
	ParseShaderStream(Stream, _type);
}

GLuint RenderShader::CompileShader(ShaderType tar)
{
	ShaderUnit* shader = GetShaderUnit(tar);
	GLuint shader_id = Shaders::CompileShaderCode(tar, shader->sh_code);
	if (shader_id == -1)
		return -1;

	shader->sh_struct->is_struct_changed = false;
	shader->sh_ID = shader_id;
	return shader_id;
}

void RenderShader::RelinkShader(ShaderType tar /*= NONE_SHADER*/)
{
	GLuint shader_id = CompileShader(tar);
	if (shader_id == -1)
		return;

	GLuint program_id = glCreateProgram();

	glAttachShader(program_id, shader_id);
	glAttachShader(program_id, GetShaderID((ShaderType)(1 - tar)));

	glLinkProgram(program_id);
	glValidateProgram(program_id);

	int link_state = -1;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_state);

	if (link_state != GL_TRUE) {
		DEBUG("Shader Link Error");
		return;
	}

	ResetID(tar, shader_id);
	_resetProgramID(program_id);
	ResetCache();

	is_shader_changed = true;
}

void RenderShader::GenerateShader(ShaderType tar /*= NONE_SHADER*/)
{
	ShaderUnit* shader = GetShaderUnit(tar);

	if (!shader->sh_struct->is_struct_changed)
		return;

	shader->sh_struct->is_struct_changed = false;
	shader->sh_code = shader->sh_struct->GenerateShader();
}

Shaders::ShaderUnit* RenderShader::GetShaderUnit(ShaderType tar /*= NONE_SHADER*/)
{
	if (tar == VERTEX_SHADER || tar == FRAGMENT_SHADER)
		return &shader_data[tar];
	else return nullptr;
}

GLuint RenderShader::GetShaderID(ShaderType type) const
{
	if (type == VERTEX_SHADER || type == FRAGMENT_SHADER)
		return shader_data[type].sh_ID;
	else return -1;
}

void RenderShader::ResetID(ShaderType type, GLuint id)
{
	ShaderUnit* shader = GetShaderUnit(type);
	if (shader->sh_ID != id) {
		glDeleteShader(shader->sh_ID);
	}
	shader->sh_ID = id;
}

void RenderShader::LocalDebug() const
{
#ifdef _DEBUG
	for (const auto& [name, loc] : _uniforms_cache)
		DEBUG(name + " : " + std::to_string(loc));
#endif
}
