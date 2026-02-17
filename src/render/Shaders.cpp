#include "Shaders.h"
#include "operator.h"
#include "structs.h"
#include "macros.h"

std::string const Shaders::shader_type[4] = { "Vertex Shader", "Fragment Shader", "Compute Shader", "Geometry Shader" };

void Shaders::ShaderUnit::_del()
{
	glDeleteShader(sh_ID);
	sh_ID = 0;
}

void Shaders::ShaderUnit::_copyInfo(const ShaderUnit& unit)
{
	sh_type = unit.sh_type;
	sh_name = unit.sh_name;
	sh_ID = unit.sh_ID;
	sh_code = unit.sh_code;
}

void Shaders::ShaderUnit::_resetID(GLuint ID)
{
	if (ID > 0 && ID != sh_ID)
		_del();
	sh_ID = ID;
}

Shaders::ShaderUnit::ShaderUnit(const ShaderUnit& unit)
{
	_resetID(unit.sh_ID);
	_copyInfo(unit);

	if (unit.sh_struct)
		sh_struct = unit.sh_struct;
}

Shaders::ShaderUnit::ShaderUnit(ShaderUnit&& unit) noexcept
{
	_resetID(unit.sh_ID);
	_copyInfo(unit);
	unit.sh_ID = 0;

	if (unit.sh_struct)
		sh_struct = std::move(unit.sh_struct);
}

Shaders::ShaderUnit::ShaderUnit(ShaderType type, std::string name)
	:sh_type(type), sh_name(name)
{}

Shaders::ShaderUnit::~ShaderUnit()
{
	if (sh_ID != 0)
		_del();
}

Shaders::ShaderUnit& Shaders::ShaderUnit::operator=(ShaderUnit&& unit) noexcept
{
	_resetID(unit.sh_ID);
	_copyInfo(unit);
	unit.sh_ID = 0;

	if (unit.sh_struct)
		sh_struct = std::move(unit.sh_struct);

	return *this;
}

Shaders::ShaderUnit& Shaders::ShaderUnit::operator=(const ShaderUnit& unit)
{
	_resetID(unit.sh_ID);
	_copyInfo(unit);

	if (unit.sh_struct)
		sh_struct = unit.sh_struct;

	return *this;
}

std::size_t Shaders::ShaderUnit::hash_fn::operator()(const ShaderUnit& inp) const
{
	static std::hash<std::string> hasher;
	return hasher(inp.sh_name + std::to_string(inp.sh_type));
}

std::string Shaders::folder_root = "res/shaders/";
std::vector<std::string> Shaders::file_type = { ".vert", ".frag", ".comp", ".geom" };

GLuint Shaders::CompileShaderCode(ShaderType _type, const std::string& source) {

	const auto [name, fname, glname] = Shaders::ParseShaderType(_type);

	GLuint id = glCreateShader(glname);
	const char* src = source.c_str(); //传入指针，需要保证指向source（shader代码）的内存一直存在

	glShaderSource(id, 1, &src, nullptr);

	//std::cout << id << std::endl;
	glCompileShader(id);

	//delete src;
	int status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	if (!status) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		std::string message;
		message.resize(length);

		glGetShaderInfoLog(id, length, &length, message.data());
		std::cout << name + " error\n";
		std::cout << message << "\n";
		DEBUG(source);
		glDeleteShader(id);
		return -1;
	}
#ifdef _DEBUG
	else
		std::cout << name << " is complied successfully!\n";
#endif
	//delete src;
	return id;
}

std::string Shaders::ReadShaderFile(ShaderType _type, const std::string& name)
{
	std::string file_name = name.find(Shaders::folder_root) == std::string::npos ? Shaders::folder_root + name : name;

	if (Shaders::ParseFileEXT(file_name) == NONE_SHADER)
		file_name += Shaders::file_type[_type];

	std::ifstream File(file_name);
	std::stringstream Stream;

	Stream << File.rdbuf();

	File.close();

	return Stream.str();
}

Shaders::Shaders(const Shaders& shader)
{
	_resetProgramID(shader.GetProgramID());
}

Shaders::Shaders(Shaders&& shader) noexcept
{
	_resetProgramID(shader.GetProgramID());
	shader.program_id = 0;
}

Shaders::~Shaders()
{

}

Shaders& Shaders::operator=(Shaders&& shader) noexcept
{
	_resetProgramID(shader.GetProgramID());
	shader.program_id = 0;

	return *this;
}

Shaders& Shaders::operator=(const Shaders& shader)
{
	_resetProgramID(shader.GetProgramID());

	return *this;
}

void Shaders::_del()
{
	glDeleteProgram(program_id);
	program_id = 0;
}

Shaders::ShaderConstInfo Shaders::ParseShaderType(ShaderType _type)
{
	switch (_type)
	{
	case VERTEX_SHADER:
		return { Shaders::shader_type[_type], ".vert", GL_VERTEX_SHADER};
	case FRAGMENT_SHADER:
		return { Shaders::shader_type[_type], ".frag", GL_FRAGMENT_SHADER };
	case COMPUTE_SHADER:
		return { Shaders::shader_type[_type], ".comp", GL_COMPUTE_SHADER };
	case GEOMETRY_SHADER:
		return { Shaders::shader_type[_type], ".geom", GL_GEOMETRY_SHADER };
	default:
		return { "/", "/", GL_NONE };
	}
}

ShaderType Shaders::ParseFileEXT(std::string path)
{
	for (int i = 0; auto & ext : Shaders::file_type) {
		if (path.find(ext) != std::string::npos)
			return ShaderType(i);
		i++;
	}

	return NONE_SHADER;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void Shaders::UseShader() const
{
	glUseProgram(program_id);
}

void Shaders::UnuseShader() const
{
	glUseProgram(0);
}

GLuint Shaders::getVarID(const char* name) const
{
	//std::cout << program_id << "\n";
	if (_uniforms_cache.find(name) != _uniforms_cache.end())
		return _uniforms_cache[name];

	this->UseShader();
	int id = glGetUniformLocation(program_id, name);

#ifdef _DEBUG
	if (id == -1)std::cout << name << " do not exist!" << std::endl;
#endif

	_uniforms_cache[name] = id;
	return glGetUniformLocation(program_id, name);
}

void Shaders::SetValue(const std::string& name, int v0, int v1, int v2, int v3)
{
	int id = getVarID(name.c_str());
	glUniform4i(id, v0, v1, v2, v3);
}

void Shaders::SetValue(const std::string& name, int v0, int v1, int v2)
{
	int id = getVarID(name.c_str());
	glUniform3i(id, v0, v1, v2);
}

void Shaders::SetValue(const std::string& name, float v0, float v1)
{
	int id = getVarID(name.c_str());
	glUniform2f(id, v0, v1);
}

void Shaders::SetValue(const std::string& name, float v0, float v1, float v2)
{
	int id = getVarID(name.c_str());
	glUniform3f(id, v0, v1, v2);
}

void Shaders::SetValue(const std::string& name, float v0, float v1, float v2, float v3)
{
	int id = getVarID(name.c_str());
	glUniform4f(id, v0, v1, v2, v3);
}

void Shaders::SetValue(const std::string& name, const glm::mat4& projection)
{
	int id = getVarID(name.c_str());
	glUniformMatrix4fv(id, 1, GL_FALSE, &projection[0][0]);
}

void Shaders::SetValue(const std::string& name, float v0)
{
	int id = getVarID(name.c_str());
	glUniform1f(id, v0);
}

void Shaders::SetValue(const std::string& name, int v0)
{
	int id = getVarID(name.c_str());
	glUniform1i(id, v0);
}

void Shaders::SetValue(const std::string& name, bool v0)
{
	int id = getVarID(name.c_str());
	glUniform1i(id, v0);
}

void Shaders::SetValue(const std::string& name, const GLuint& v0)
{
	int id = getVarID(name.c_str());
	glUniform1i(id, v0);
}

void Shaders::SetValue(const std::string& name, const glm::vec2& vec2)
{
	SetValue(name, vec2[0], vec2[1]);
}

void Shaders::SetValue(const std::string& name, const glm::vec3& vec3)
{
	SetValue(name, vec3[0], vec3[1], vec3[2]);
}

void Shaders::SetValue(const std::string& name, const glm::vec4& vec4)
{
	SetValue(name, vec4[0], vec4[1], vec4[2], vec4[3]);
}

void Shaders::SetValue(const std::string& name, GLsizei count, const float* va0, ArrayType TYPE)
{
	int id = getVarID(name.c_str());
	switch (TYPE)
	{
	case NULL_ARRAY:

		break;
	case VEC1_ARRAY:
		glUniform1fv(id, count, va0);
		break;
	case VEC2_ARRAY:
		glUniform2fv(id, count * 2, va0);
		break;
	case VEC3_ARRAY:
		glUniform3fv(id, count * 3, va0);
		break;
	case VEC4_ARRAY:
		glUniform4fv(id, count * 4, va0);
		break;
	case MAT4_ARRAY:
		glUniform4fv(id, count * 4, va0);
		break;
	default:
		break;
	}
}

void Shaders::SetValue(const std::string& name, GLsizei count, const int* va0, ArrayType TYPE)
{
	int id = getVarID(name.c_str());
	switch (TYPE)
	{
	case NULL_ARRAY:

		break;
	case VEC1_ARRAY:
		glUniform1iv(id, count, va0);
		break;
	case VEC2_ARRAY:
		glUniform2iv(id, count * 2, va0);
		break;
	case VEC3_ARRAY:
		glUniform3iv(id, count * 3, va0);
		break;
	case VEC4_ARRAY:
		glUniform4iv(id, count * 4, va0);
		break;
	case MAT4_ARRAY:
		glUniform4iv(id, count * 4, va0);
		break;
	default:
		break;
	}
}

void Shaders::SetValue(const std::string& name, GLsizei count, const GLuint* va0, ArrayType TYPE)
{
	int id = getVarID(name.c_str());
	switch (TYPE)
	{
	case NULL_ARRAY:

		break;
	case VEC1_ARRAY:
		glUniform1uiv(id, count, va0);
		break;
	case VEC2_ARRAY:
		glUniform2uiv(id, count * 2, va0);
		break;
	case VEC3_ARRAY:
		glUniform3uiv(id, count * 3, va0);
		break;
	case VEC4_ARRAY:
		glUniform4uiv(id, count * 4, va0);
		break;
	case MAT4_ARRAY:
		glUniform4uiv(id, count * 4, va0);
		break;
	default:
		break;
	}
}

void Shaders::SetValue(const std::string& name, ArrayUni arr)
{
	SetValue(name, arr.size, arr.data, arr.type);
}

void Shaders::SetValue(const std::string& name, GLsizei count, const glm::mat4* va0)
{
	int id = getVarID(name.c_str());
	glUniformMatrix4fv(id, count, GL_FALSE, (GLfloat*)va0);
}

void Shaders::SetValue(const Material* mat)
{
	for (const auto& [ptype, pdata] : mat->mat_params) {

		const auto& [dtype, dfloat, dcol, _] = pdata;
		switch (dtype)
		{
		case Material::MPARA_FLT:
			SetValue("U_" + Material::mat_uniform_name[ptype], dfloat);
			break;
		case Material::MPARA_COL:
			SetValue("U_" + Material::mat_uniform_name[ptype], dcol);
			break;
		case Material::MPARA_TEX:
			SetValue("U_" + Material::mat_uniform_name[ptype], ptype);
			break;
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

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

#include <algorithm>
void RenderShader::UpdateMaterial(Material* mat)
{
	assert(shader_data[FRAGMENT_SHADER].sh_struct.has_value());
	DEBUG("Updating material: " + mat->mat_name);

	ShaderStruct& sh_struct = *shader_data[FRAGMENT_SHADER].sh_struct;
	for (const auto& [ptype, pdata] : mat->mat_params) {
		const auto& [dtype, dfloat, dcol, dtex] = pdata;

		if (ptype == Material::MAT_NORMAL || ptype == Material::MAT_BUMP)
			continue;  // TODO: implement in the future

		auto loc_const = std::find_if(
			sh_struct.const_list.begin(),
			sh_struct.const_list.end(),
			[ptype](const S_const& con) ->bool {
				return std::get<1>(con) == "m_" + Material::mat_uniform_name[ptype];     // e.g. Material::MAT_ALBEDO -> m_albedo
			}
		);

		auto loc_uniform = std::find_if(
			sh_struct.uniform_list.begin(),
			sh_struct.uniform_list.end(),
			[ptype](const S_U& uni) ->bool {
				return std::get<0>(uni) == "U_" + Material::mat_uniform_name[ptype];     // e.g. Material::MAT_ALBEDO -> m_albedo
			}
		);

		switch (dtype)
		{
		case Material::MPARA_FLT:

			if (loc_uniform != sh_struct.uniform_list.end())
				std::get<1>(*loc_uniform) = FLOAT_PARA;
			else
				sh_struct.SetUni(FLOAT_PARA, 1, "U_" + Material::mat_uniform_name[ptype]);

			if (loc_const != sh_struct.const_list.end())
				*loc_const = ShaderLib::v_material[ptype];
			else
				sh_struct.const_list.emplace_back(ShaderLib::v_material[ptype]);

			break;
		case Material::MPARA_COL:

			if (loc_uniform != sh_struct.uniform_list.end())
				std::get<1>(*loc_uniform) = VEC3_PARA;
			else
				sh_struct.SetUni(VEC3_PARA, 1, "U_" + Material::mat_uniform_name[ptype]);

			if (loc_const != sh_struct.const_list.end())
				*loc_const = ShaderLib::v_material[ptype];
			else
				sh_struct.const_list.emplace_back(ShaderLib::v_material[ptype]);

			break;
		case Material::MPARA_TEX:

			if (loc_uniform != sh_struct.uniform_list.end())
				std::get<1>(*loc_uniform) = TEXTURE_PARA;
			else
				sh_struct.SetUni(TEXTURE_PARA, 1, "U_" + Material::mat_uniform_name[ptype]);

			if (loc_const != sh_struct.const_list.end())
				*loc_const = ShaderLib::t_material[ptype];
			else
				sh_struct.const_list.emplace_back(ShaderLib::t_material[ptype]);

			break;
		}
	}

	sh_struct.is_struct_changed = true;
	is_shader_changed = true;

	GenerateShader(FRAGMENT_SHADER);
	RelinkShader(FRAGMENT_SHADER);
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




///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, std::shared_ptr<ComputeShader>> ComputeShader::comp_list = {};
std::unordered_map<std::string, std::vector<ComputeShader::Default>> ComputeShader::config_list = {};

void ComputeShader::PushDefult(std::string name, std::string para_name, AvailUnis def)
{
	if (config_list.find(name) == config_list.end())
		config_list[name] = std::vector<ComputeShader::Default>();

	config_list[name].emplace_back(para_name, def);
}

void ComputeShader::PushDefult(std::string name, std::string para_name, GLuint _size, float* _data, ArrayType _type)
{
	PushDefult(name, para_name, AvailUnis(Shaders::ArrayUni(_size, _data, _type)));
}

#include "xdz_math.h"
void ComputeShader::InitComputeLib(RenderConfigs* config)
{
	static auto pos_offset = xdzm::rand3nv(16); // must be static

	ComputeShader::ImportShaderConfigs("shadow/Shadow_Point", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Sun", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Spot", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Area", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));

	ComputeShader::ImportShaderConfigs("shadow/Shadow_Point_SDF", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Sun_SDF", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	// TODO: Shadow_Spot_SDF
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Area_SDF", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));

	// TODO: Shadow_Area_VSSM
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Point_VSSM", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	// TODO: Shadow_Spot_VSSM
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Sun_VSSM", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	
	static std::vector<glm::vec3> kernel = xdzm::rand3hKernel(config->r_ao_ksize);

	for (const auto& pref : ShaderLib::AO_prefix)
		ComputeShader::ImportShaderConfigs("pps/" + pref + "AO", Uni("incre_average", true), Uni("kernel_length", GLuint(config->r_ao_ksize)), Uni("kernel", GLuint(config->r_ao_ksize), (float*)kernel.data(), VEC3_ARRAY), Uni("noise_size", 16), Uni("radius", config->r_ao_radius), Uni("U_opt_flow", 1));

	for (const auto& pref : ShaderLib::SSR_prefix)
		ComputeShader::ImportShaderConfigs("pps/SSR" + pref, Uni("U_pos", 1), Uni("U_dir_diff", 7), Uni("U_dir_spec", 8), Uni("U_ind_diff", 9), Uni("U_ind_spec", 10), Uni("U_emission", 11), Uni("U_opt_flow", 12), Uni("LTC1", 13));
}

void ComputeShader::ResetComputeLib()
{
	comp_list.clear();
	config_list.clear();
}

ComputeShader::ComputeShader(const std::string& name)
{
	comp_shader.sh_name = name;

	std::string code = Shaders::ReadShaderFile(COMPUTE_SHADER, name);
	CreateShader(code);

	comp_shader.sh_code = code;
	comp_shader.sh_type = COMPUTE_SHADER;

	ResetDefult(name);
}

ComputeShader::ComputeShader()
{}

ComputeShader::~ComputeShader()
{

}

void ComputeShader::ResetID(ShaderType tar, GLuint _id)
{
	if (comp_shader.sh_ID != _id)
		glDeleteShader(comp_shader.sh_ID);
	comp_shader.sh_ID = _id;
}

void ComputeShader::ResetDefult(std::string name)
{

	if (ComputeShader::config_list.find(name) == ComputeShader::config_list.end()) {
		DEBUG("no defult configs for: " + name);
		return;
	}

	UseShader();
	for (auto& [p_name, def] : ComputeShader::config_list[name]) {
		std::visit([this, p_name](auto& p_value) {SetValue(p_name, p_value); }, def);
	}
}

void ComputeShader::CreateShader(const std::string& compShader)
{
	program_id = glCreateProgram();

	comp_shader.sh_ID = CompileShaderCode(COMPUTE_SHADER, compShader);

	glAttachShader(program_id, comp_shader.sh_ID);
	glLinkProgram(program_id);

	GLint linked = 0;
	glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
	if (!linked) {
		char log[4096];
		glGetProgramInfoLog(program_id, sizeof(log), nullptr, log);
		printf("Link error:\n%s\n", log);
	}
}

void ComputeShader::ParseShaderCode(const std::string& _code, ShaderType tar)
{
	if (tar == COMPUTE_SHADER)
		comp_shader.sh_code = _code;
}

void ComputeShader::RelinkShader(ShaderType tar)
{
	GLint program_id = glCreateProgram();
	GLint shader_id = CompileShaderCode(COMPUTE_SHADER, comp_shader.sh_code);

	glAttachShader(program_id, shader_id);
	glLinkProgram(program_id);

	_resetProgramID(program_id);
	ResetID(COMPUTE_SHADER, shader_id);

	GLint linked = 0;
	glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
	if (!linked) {
		char log[4096];
		glGetProgramInfoLog(program_id, sizeof(log), nullptr, log);
		printf("Link error:\n%s\n", log);
	}

	ResetDefult(comp_shader.sh_name);
}

Shaders::ShaderUnit* ComputeShader::GetShaderUnit(ShaderType tar /*= NONE_SHADER*/)
{
	if (tar != COMPUTE_SHADER)
		return nullptr;

	return &comp_shader;
}

void ComputeShader::RunComputeShaderSCR(const glm::vec2& _scr_size, GLuint _batch, bool _edge_fix /*= true*/)
{
	RunComputeShader(_scr_size / _batch + (_edge_fix ? glm::vec2(1) : glm::vec2(0)));
}

void ComputeShader::RunComputeShader(GLuint workgroup_count_x /*= 1*/, GLuint workgroup_count_y /*= 1*/, GLuint workgroup_count_z /*= 1*/) const
{
	glDispatchCompute(workgroup_count_x, workgroup_count_y, workgroup_count_z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ComputeShader::RunComputeShader(const glm::vec2& _size)
{
	UseShader();
	RunComputeShader(GLuint(_size.x), GLuint(_size.y), 1);
	UnuseShader();
}

GLuint ComputeShader::GetShaderID(ShaderType type) const
{
	return comp_shader.sh_ID;
}

void ComputeShader::LocalDebug() const
{
#ifdef _DEBUG
	//DEBUG(comp_shader)
#endif // _DEBUG
}

ComputeShader& ComputeShader::ImportShader(std::string _name)
{
	return *ImportShaderSrc(_name).get();
}

std::shared_ptr<ComputeShader> ComputeShader::ImportShaderSrc(std::string _name)
{
	if (comp_list.find(_name) != comp_list.end())
		return comp_list[_name];

	comp_list[_name] = std::make_shared<ComputeShader>(_name);
	return comp_list[_name];
}

std::string ComputeShader::GetSSRShaderName(RenderConfigs* config)
{
	int alg = (int)config->r_ssr_algorithm;
	assert(alg < ShaderLib::SSR_prefix.size() && "unknown SSR type");
	return "pps/SSR" + ShaderLib::SSR_prefix[alg];
}

std::string ComputeShader::GetAOShaderName(RenderConfigs* config)
{
	int alg = (int)config->r_ao_algorithm;
	assert(alg < ShaderLib::AO_prefix.size() && "unknown AO type");
	return "pps/" + ShaderLib::AO_prefix[alg] + "AO";
}

std::string ComputeShader::GetAAShaderName(RenderConfigs* config)
{
	int alg = (int)config->r_anti_alias;
	assert(alg < ShaderLib::AA_prefix.size() && "unknown AA type");
	return "pps/" + ShaderLib::AA_prefix[alg] + "AA";
}

std::string ComputeShader::GetShadowShaderName(char _type, char _light_type)
{
	static std::string light_prefix[4] = { "_Point", "_Sun", "_Spot", "_Area" };

	assert(_type < ShaderLib::Shadow_prefix.size());
	assert(_type != 0);
	return "shadow/Shadow" + light_prefix[_light_type] + ShaderLib::Shadow_prefix[_type];
}
