#include "RenderShader.h"
#include "operator.h"
#include "structs.h"
#include "macros.h"

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

	if (_name.find(Shaders::folder_root) == std::string::npos) {
		// Check if the name already has a file extension
		if (Shaders::ParseFileEXT(_name) == NONE_SHADER) {
			// No extension found, add the appropriate one
			_name = Shaders::folder_root + _name + Shaders::file_type[_type];
		}
		else {
			// Extension already present, just add folder_root
			_name = Shaders::folder_root + _name;
		}
	}

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



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


FastLoadShader::FastLoadShader(const std::string& vert, const std::string& frag /*= ""*/)
{
	vert_name = vert;
	frag_name = frag == "" ? vert : frag;

	std::string vert_code = Shaders::ReadShaderFile(VERTEX_SHADER, vert_name);
	std::string frag_code = Shaders::ReadShaderFile(FRAGMENT_SHADER, frag_name);

	CreatShader(vert_code, frag_code);
}

FastLoadShader::FastLoadShader()
{

}

FastLoadShader::~FastLoadShader()
{

}

void FastLoadShader::CreatShader(const std::string& verShader, const std::string& fragShader)
{
	program_id = glCreateProgram();

	vs_id = CompileShaderCode(VERTEX_SHADER, verShader);
	fs_id = CompileShaderCode(FRAGMENT_SHADER, fragShader);

	glAttachShader(program_id, vs_id);
	glAttachShader(program_id, fs_id);

	glLinkProgram(program_id);
	glValidateProgram(program_id);
}

GLuint FastLoadShader::GetShaderID(ShaderType type) const
{
	switch (type)
	{
	case VERTEX_SHADER:
		return vs_id;
	case FRAGMENT_SHADER:
		return fs_id;
	default:
		return -1;
	}
}

void FastLoadShader::LocalDebug() const
{
#ifdef _DEBUG
	DEBUG(fast_shaders.verShader);
	DEBUG(fast_shaders.fragShader);
#endif
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, std::shared_ptr<ChainedShader>> ChainedShader::chain_sh_list = {};

ChainedShader::ChainedShader(const std::vector<std::string>& chain)
{
	_idx_to_type.reserve(chain.size());
	shader_chain.reserve(chain.size());
	LOOP(chain.size()) {
		shader_chain.emplace_back(Shaders::ParseFileEXT(chain[i]), chain[i]);
		_type_to_idx[shader_chain[i].sh_type] = i;
		_idx_to_type.emplace_back(shader_chain[i].sh_type);
	}

	CreatShader();
}

ChainedShader::ChainedShader()
{

}

ChainedShader::~ChainedShader()
{

}

void ChainedShader::CreatShader()
{
	program_id = glCreateProgram();

	for (auto& node : shader_chain) {
		std::string code = Shaders::ReadShaderFile(node.sh_type, node.sh_name);
		node.sh_ID = Shaders::CompileShaderCode(node.sh_type, code);
		node.sh_code = code;
	}

	for (const auto& node : shader_chain) {
		glAttachShader(program_id, node.sh_ID);
	}

	glLinkProgram(program_id);
	glValidateProgram(program_id);
}

ChainedShader& ChainedShader::ImportShader(const std::vector<std::string>& chain)
{
	std::string _name = "";
	for (auto& n : chain)
		_name += n + "-";

	if (chain_sh_list.find(_name) != chain_sh_list.end())
		return *chain_sh_list[_name].get();

	chain_sh_list[_name] = std::make_shared<ChainedShader>(chain);
	return *chain_sh_list[_name].get();
}

GLuint ChainedShader::CompileShader(ShaderType type)
{
	ShaderUnit* node = GetShaderUnit(type);
	if (node == nullptr)
		return -1;
	return Shaders::CompileShaderCode(node->sh_type, node->sh_code);
}

void ChainedShader::RelinkShader(ShaderType type)
{
	GLuint shader_id = CompileShader(type);
	if (shader_id == -1)
		return;

	GLuint program_id = glCreateProgram();

	glAttachShader(program_id, shader_id);
	for (auto& unit : shader_chain) {
		if (unit.sh_type != type)
			glAttachShader(program_id, unit.sh_ID);
	}

	glLinkProgram(program_id);
	glValidateProgram(program_id);

	int link_state = -1;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_state);

	if (link_state != GL_TRUE) {
		DEBUG("Shader Link Error");
		return;
	}

	ResetID(type, shader_id);
	_resetProgramID(program_id);
	ResetCache();

	is_shader_changed = true;
}

Shaders::ShaderUnit* ChainedShader::GetShaderUnit(ShaderType type)
{
	const int idx = _type_to_idx[type];
	if (idx == -1)
		return nullptr;
	else
		return &shader_chain[idx];
}

void ChainedShader::ParseShaderCode(const std::string& _code, ShaderType type)
{
	ShaderUnit* node = GetShaderUnit(type);
	if (node == nullptr)
		return;
	node->sh_code = _code;
}

GLuint ChainedShader::GetShaderID(ShaderType type) const
{
	const int idx = _type_to_idx[type];
	if (idx == -1)
		return -1;
	else
		return shader_chain[idx].sh_ID;
}

void ChainedShader::ResetID(ShaderType type, GLuint id)
{
	ShaderUnit* node = GetShaderUnit(type);
	if (node != nullptr) {
		if (node->sh_ID != id)
			glDeleteShader(node->sh_ID);
		node->sh_ID = id;
	}
}

void ChainedShader::LocalDebug() const
{
#if _DEBUG
	for (auto& sh : shader_chain)
		DEBUG(std::get<0>(Shaders::ParseShaderType(sh.sh_type)) + " " + sh.sh_name + " " + std::to_string(sh.sh_ID));
#endif // _DEBUG
}
