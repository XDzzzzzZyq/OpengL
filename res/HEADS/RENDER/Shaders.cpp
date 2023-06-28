#include "Shaders.h"
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

	//std::cout << status << std::endl;

	if (!status) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];

		glGetShaderInfoLog(id, length, &length, message);
		std::cout << name + " error\n";
		std::cout << message << "\n";
		DEBUG(source)
		//delete message;
		return 0;
	}
#ifdef _DEBUG
	std::cout << name << " is complied successfully!\n";
#endif
	//delete src;
	return id;
}

std::string Shaders::ReadShaderFile(ShaderType _type, const std::string& name)
{
	std::string file_name = name.find(ShaderLib::folder_root) == std::string::npos ? ShaderLib::folder_root + name : name;
	
	if (Shaders::ParseFileEXT(file_name) == NONE_SHADER)
		file_name += ShaderLib::file_type[_type];

	std::ifstream File(file_name);
	std::stringstream Stream;

	Stream << File.rdbuf();

	File.close();

	return Stream.str();
}

Shaders::ShaderConstInfo Shaders::ParseShaderType(ShaderType _type)
{
	switch (_type)
	{
	case VERTEX_SHADER:
		return { "Vertex Shader", ".vert", GL_VERTEX_SHADER };
	case FRAGMENT_SHADER:
		return { "Fragment Shader", ".frag", GL_FRAGMENT_SHADER };
	case COMPUTE_SHADER:
		return { "Compute Shader", ".comp", GL_COMPUTE_SHADER };
	case GEOMETRY_SHADER:
		return { "Geometry Shader", ".geom", GL_GEOMETRY_SHADER };
	default:
		return { "/", "/", GL_NONE };
	}
}

ShaderType Shaders::ParseFileEXT(std::string path)
{
	for (int i = 0; auto & ext : ShaderLib::file_type) {
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

void Shaders::DelShad()
{
	glDeleteProgram(program_id);
	program_id = 0;
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

void Shaders::SetValue(const std::string& name, int v0, int v1, int v2, float v3)
{
	int id = getVarID(name.c_str());
	glUniform4f(id, v0, v1, v2, v3);
}

void Shaders::SetValue(const std::string& name, int v0, int v1, int v2)
{
	int id = getVarID(name.c_str());
	glUniform3f(id, v0, v1, v2);
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
	int id = getVarID(name.c_str());
	glUniform2f(id, vec2[0], vec2[1]);
}

void Shaders::SetValue(const std::string& name, const glm::vec3& vec3)
{
	int id = getVarID(name.c_str());
	glUniform3f(id, vec3[0], vec3[1], vec3[2]);
}

void Shaders::SetValue(const std::string& name, const glm::vec4& vec4)
{
	int id = getVarID(name.c_str());
	glUniform4f(id, vec4[0], vec4[1], vec4[2], vec4[3]);
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

void Shaders::SetValue(const std::string& name, GLsizei count, const glm::mat4* va0)
{
	int id = getVarID(name.c_str());
	glUniformMatrix4fv(id, count, GL_FALSE, (GLfloat*)va0);
}

void Shaders::SetValue(Material* mat)
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

	vs_id = CompileShaderCode(VERTEX_SHADER, verShader);
	fs_id = CompileShaderCode(FRAGMENT_SHADER, fragShader);

	glAttachShader(program_id, vs_id);
	glAttachShader(program_id, fs_id);

	glLinkProgram(program_id);
	glValidateProgram(program_id);

	//glDeleteShader(vs_id);
	//glDeleteShader(fs_id);
	//glDeleteProgram(program_id);
}

#include <algorithm>
void RenderShader::UpdateMaterial(Material* mat)
{
	ShaderStruct& sh_struct = shader_struct_list[FRAGMENT_SHADER];
	for (const auto& [ptype, pdata] : mat->mat_params) {
		const auto& [dtype, dfloat, dcol, dtex] = pdata;

		if(ptype == MAT_NORMAL || ptype == MAT_BUMP)
			continue;  // skip them currently

		auto loc_const = std::find_if(
			sh_struct.const_list.begin(),
			sh_struct.const_list.end(),
			[ptype](const S_const& con) ->bool {
				return std::get<1>(con) == "m_" + Material::mat_uniform_name[ptype];     // e.g. MAT_ALBEDO -> m_albedo
			}
		);

		auto loc_uniform = std::find_if(
			sh_struct.uniform_list.begin(),
			sh_struct.uniform_list.end(),
			[ptype](const S_U& uni) ->bool {
				return std::get<0>(uni) == "U_" + Material::mat_uniform_name[ptype];     // e.g. MAT_ALBEDO -> m_albedo
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
	vert_name = vert;
	frag_name = frag == "" ? vert : frag;

	ParseShaderFile(vert_name, VERTEX_SHADER);
	ParseShaderFile(frag_name, FRAGMENT_SHADER);
	GenerateShader();
	CreatShader(shader_list[VERTEX_SHADER], shader_list[FRAGMENT_SHADER]);
}

RenderShader::RenderShader()
{
}

RenderShader::~RenderShader()
{
}

void RenderShader::ResetID(ShaderType type, GLuint id)
{
	switch (type)
	{
	case NONE_SHADER:
		program_id = id;
		break;
	case VERTEX_SHADER:
		vs_id = id;
		break;
	case FRAGMENT_SHADER:
		fs_id = id;
		break;
	default:
		break;
	}
}

void RenderShader::ParseShaderCode(const std::string& _code, ShaderType _type)
{
	shader_struct_list[_type].Reset();
	_LINK_LOC = {};
	if (_code.empty()) {
		std::stringstream Stream(shader_list[_type]);
		ParseShaderStream(Stream, _type);
	}
	else {
		shader_list[_type] = _code;
		std::stringstream Stream(_code);
		ParseShaderStream(Stream, _type);
	}
}

GLuint RenderShader::CompileShader(ShaderType tar)
{
	const char* src = shader_list[tar].c_str(); //传入指针，需要保证指向source（shader代码）的内存一直存在
	GLuint shader_id = glCreateShader(tar == 0 ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

	glShaderSource(shader_id, 1, &src, nullptr);

	//std::cout << id << std::endl;
	glCompileShader(shader_id);

	//delete src;
	int status = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);

	//std::cout << status << std::endl;
	std::string type = tar == VERTEX_SHADER ? "Vertex" : "Frag";
	if (!status) {
		int length;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];

		glGetShaderInfoLog(shader_id, length, &length, message);
		std::cout << type + " shader error\n";
		std::cout << message << "\n";

	}
	else {
		std::cout << type << " is complied successfully!" << std::endl;
		shader_struct_list[tar].is_struct_changed = false;
	}
	tar == 0 ? vs_id = shader_id : fs_id = shader_id;
	return shader_id;
}

void RenderShader::RelinkShader(ShaderType tar /*= NONE_SHADER*/)
{
	glDeleteProgram(getProgramID());
	glDeleteShader(getShaderID(tar));

	GLuint program_id = glCreateProgram();

	GLuint shader_id = CompileShader(tar);
	glAttachShader(program_id, shader_id);
	glAttachShader(program_id, getShaderID((ShaderType)(1-tar)));

	glLinkProgram(program_id);
	glValidateProgram(program_id);

	int link_state = -1;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_state);

	if (link_state != GL_TRUE)
		DEBUG("Shader Link Error")

	ResetID(tar, shader_id);
	ResetID(NONE_SHADER, program_id);
	ResetCache();

	is_shader_changed = true;
}

GLuint RenderShader::getShaderID(ShaderType type) const
{
	switch (type)
	{
	case VERTEX_SHADER:
		return vs_id;
	case FRAGMENT_SHADER:
		return fs_id;
	default:
		return 0;
	}
}

void RenderShader::LocalDebug() const
{
#ifdef _DEBUG
	for (const auto& [name, loc] : _uniforms_cache)
		DEBUG(name + " : " + std::to_string(loc))
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

GLuint FastLoadShader::getShaderID(ShaderType type) const
{
	switch (type)
	{
	case VERTEX_SHADER:
		return vs_id;
	case FRAGMENT_SHADER:
		return fs_id;
	default:
		return 0;
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
	for (auto& path : chain)
		shader_chain.emplace_back(Shaders::ParseFileEXT(path), path, 0);

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
		std::string code = Shaders::ReadShaderFile(node.type, node.name);
		node.ID = Shaders::CompileShaderCode(node.type, code);
	}

	for (const auto& node : shader_chain) {
		glAttachShader(program_id, node.ID);
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

GLuint ChainedShader::getShaderID(ShaderType type) const
{
	for (auto& sh : shader_chain)
		if (sh.type == type)
			return sh.ID;
	return 0;
}

void ChainedShader::LocalDebug() const
{
#ifdef _DEBUG
	for (auto& sh : shader_chain)
		DEBUG(std::get<0>(Shaders::ParseShaderType(sh.type)) + " " + sh.name + " " + std::to_string(sh.ID));
#endif // _DEBUG
}




///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, std::shared_ptr<ComputeShader>> ComputeShader::comp_list = {};

void ComputeShader::ResetComputeLib()
{
	for (auto& [name, sh] : comp_list)
		sh->DelShad();
}

ComputeShader::ComputeShader(const std::string& name)
	:comp_name(name)
{
	std::string code = Shaders::ReadShaderFile(COMPUTE_SHADER, name);
	CreateShader(code);
}

ComputeShader::ComputeShader()
{

}

ComputeShader::~ComputeShader()
{

}

void ComputeShader::CreateShader(const std::string& compShader)
{
	program_id = glCreateProgram();

	comp_id = CompileShaderCode(COMPUTE_SHADER, compShader);

	glAttachShader(program_id, comp_id);

	glLinkProgram(program_id);
}

GLuint ComputeShader::CompileShader()
{
	return comp_id;
}

void ComputeShader::RunComputeShaderSCR(const ImVec2& _scr_size, GLuint _batch, bool _edge_fix /*= true*/)
{
	RunComputeShader(_scr_size / _batch + (_edge_fix ? glm::vec2(1) : glm::vec2(0)));
}

void ComputeShader::RunComputeShader(GLuint workgroup_count_x /*= 1*/, GLuint workgroup_count_y /*= 1*/, GLuint workgroup_count_z /*= 1*/) const
{
	glDispatchCompute(workgroup_count_x, workgroup_count_y, workgroup_count_z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ComputeShader::RunComputeShader(const ImVec2& _size)
{
	UseShader();
	RunComputeShader(_size[0], _size[1], 1);
	UnuseShader();
}

GLuint ComputeShader::getShaderID(ShaderType type) const
{
	return comp_id;
}

void ComputeShader::LocalDebug() const
{
#ifdef _DEBUG
	//DEBUG(comp_shader)
#endif // _DEBUG
}

ComputeShader& ComputeShader::ImportShader(std::string _name)
{
	if (comp_list.find(_name) != comp_list.end())
		return *comp_list[_name].get();

	comp_list[_name] = std::make_shared<ComputeShader>(_name);
	return *comp_list[_name].get();
}
