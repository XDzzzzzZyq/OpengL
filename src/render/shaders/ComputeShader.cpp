#include "ComputeShader.h"
#include "operator.h"
#include "structs.h"
#include "macros.h"
#include "ShaderLib.h"
#include "xdz_math.h"

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
