#include "Shaders.h"
#include "operator.h"
#include "structs.h"
#include "macros.h"

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
	std::string file_name = name.find(Shaders::folder_root) == std::string::npos ? Shaders::folder_root + name : name;
	
	if (Shaders::ParseFileEXT(file_name) == NONE_SHADER)
		file_name += Shaders::file_type[_type];

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

	ShaderStruct& sh_struct = *shader_data[FRAGMENT_SHADER].sh_struct;
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
	shader_data[VERTEX_SHADER].sh_name = vert;
	shader_data[FRAGMENT_SHADER].sh_name = frag == "" ? vert : frag;

	shader_data[VERTEX_SHADER].sh_struct = ShaderStruct();
	shader_data[FRAGMENT_SHADER].sh_struct = ShaderStruct();

	shader_data[VERTEX_SHADER].sh_type = VERTEX_SHADER;
	shader_data[FRAGMENT_SHADER].sh_type = FRAGMENT_SHADER;

	ParseShaderFile(shader_data[VERTEX_SHADER].sh_name, VERTEX_SHADER);
	ParseShaderFile(shader_data[FRAGMENT_SHADER].sh_name, FRAGMENT_SHADER);

	GenerateShader(FRAGMENT_SHADER);
	GenerateShader(VERTEX_SHADER);

	CreatShader(shader_data[VERTEX_SHADER].sh_code, shader_data[FRAGMENT_SHADER].sh_code);
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
	case FRAGMENT_SHADER:
		shader_data[type].sh_ID = id;
		break;
	default:
		break;
	}
}

void RenderShader::ParseShaderFile(std::string _name, ShaderType _type) {
	Timer timer("ParseShader");

	active_shader = _type;

	shader_data[active_shader].sh_struct->is_struct_changed = false;

	if (_name.find(Shaders::folder_root) == std::string::npos)
		_name = Shaders::folder_root + _name + Shaders::file_type[_type];

	std::ifstream Stream(_name);

	ParseShaderStream(Stream, active_shader);

	shader_data[active_shader].sh_struct->func_list_state.resize(shader_data[active_shader].sh_struct->func_list.size());


	std::cout << "shaders are loaded up successfully!" << std::endl;
	//m_shaders = { shaders[0].str(),shaders[1].str() };
}

void RenderShader::ParseShaderCode(const std::string& _code, ShaderType _type)
{
	shader_data[_type].sh_struct->Reset();
	_LINK_LOC = {};
	if (!_code.empty()) 
		shader_data[_type].sh_code = _code;

	std::stringstream Stream(shader_data[_type].sh_code);
	ParseShaderStream(Stream, _type);
}

GLuint RenderShader::CompileShader(ShaderType tar)
{
	const char* src = shader_data[tar].sh_code.c_str(); //传入指针，需要保证指向source（shader代码）的内存一直存在
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
		shader_data[tar].sh_struct->is_struct_changed = false;
	}
	shader_data[tar].sh_ID = shader_id;
	return shader_id;
}

void RenderShader::RelinkShader(ShaderType tar /*= NONE_SHADER*/)
{
	glDeleteProgram(GetProgramID());
	glDeleteShader(GetShaderID(tar));

	GLuint program_id = glCreateProgram();

	GLuint shader_id = CompileShader(tar);
	glAttachShader(program_id, shader_id);
	glAttachShader(program_id, GetShaderID((ShaderType)(1-tar)));

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

void RenderShader::GenerateShader(ShaderType tar /*= NONE_SHADER*/)
{
	assert(tar != NONE_SHADER);

	if (!shader_data[tar].sh_struct->is_struct_changed)
		return;

	shader_data[tar].sh_struct->is_struct_changed = false;
	shader_data[tar].sh_code = shader_data[tar].sh_struct->GenerateShader();
}

Shaders::ShaderUnit* RenderShader::GetShaderUnit(ShaderType tar /*= NONE_SHADER*/)
{
	assert(tar != NONE_SHADER);

	return &shader_data[tar];
}

GLuint RenderShader::GetShaderID(ShaderType type) const
{
	switch (type)
	{
	case VERTEX_SHADER:
	case FRAGMENT_SHADER:
		return shader_data[type].sh_ID;
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

GLuint FastLoadShader::GetShaderID(ShaderType type) const
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
		shader_chain.emplace_back(Shaders::ParseFileEXT(path), path);

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

GLuint ChainedShader::GetShaderID(ShaderType type) const
{
	for (auto& sh : shader_chain)
		if (sh.sh_type == type)
			return sh.sh_ID;
	return 0;
}

void ChainedShader::LocalDebug() const
{
#ifdef _DEBUG
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

	ComputeShader::ImportShaderConfigs("shadow/Shadow_Point",Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Sun",  Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Spot", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Area", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));

	ComputeShader::ImportShaderConfigs("shadow/Shadow_Point_SDF", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));
	ComputeShader::ImportShaderConfigs("shadow/Shadow_Area_SDF", Uni("U_opt_flow", 6), Uni("Shadow_Map", 31));

	static std::vector<glm::vec3> kernel = xdzm::rand3hKernel(config->r_ao_ksize);

	for (const auto& pref : ShaderLib::AO_prefix)
		ComputeShader::ImportShaderConfigs("pps/"+pref + "AO", Uni("incre_average", true), Uni("kernel_length", (GLuint)config->r_ao_ksize), Uni("kernel", (GLuint)config->r_ao_ksize, (float*)kernel.data(), VEC3_ARRAY), Uni("noise_size", 16), Uni("radius", config->r_ao_radius), Uni("U_opt_flow", 1));

	for(const auto& pref : ShaderLib::SSR_prefix)
		ComputeShader::ImportShaderConfigs("pps/SSR"+ pref, Uni("U_pos", 1), Uni("U_dir_diff", 7), Uni("U_dir_spec", 8), Uni("U_ind_diff", 9), Uni("U_ind_spec", 10), Uni("U_emission", 11), Uni("U_opt_flow", 12), Uni("LTC1", 13));
}

void ComputeShader::ResetComputeLib()
{
	for (auto& [name, sh] : comp_list)
		sh->DelShad();
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
	RunComputeShader(_size.x, _size.y, 1);
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
	int alg = (int)config->r_shadow_algorithm;
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
	return "shadow/Shadow" + light_prefix[_light_type] + ShaderLib::Shadow_prefix[_type];
}
