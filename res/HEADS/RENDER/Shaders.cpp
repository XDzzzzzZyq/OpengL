#include "Shaders.h"
GLuint CompileShaderFile(GLuint TYPE, const std::string& source) {
	GLuint id = glCreateShader(TYPE);
	const char* src = source.c_str(); //传入指针，需要保证指向source（shader代码）的内存一直存在

	glShaderSource(id, 1, &src, nullptr);

	//std::cout << id << std::endl;
	glCompileShader(id);

	//delete src;
	int status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	//std::cout << status << std::endl;
	const std::string type = Shaders::GetShaderTypeName(TYPE);
	if (!status) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];

		glGetShaderInfoLog(id, length, &length, message);
		std::cout << type + " error\n";
		std::cout << message << "\n";
		DEBUG(source)
		//delete message;
		return 0;
	}
#ifdef _DEBUG
	std::cout << type << " is complied successfully!\n";
#endif
	//delete src;
	return id;
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

void Shaders::DelShad() const
{
	glDeleteProgram(program_id);
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

const char* Shaders::GetShaderTypeName(GLuint _Type, bool _using_filename)
{
	switch (_Type)
	{
	case GL_VERTEX_SHADER:
		return _using_filename ? ".vert" : "Vertex Shader"; break;
	case GL_FRAGMENT_SHADER:
		return _using_filename ? ".frag" : "Fragment Shader"; break;
	case GL_COMPUTE_SHADER:
		return _using_filename ? ".comp" : "Compute Shader"; break;
	default:
		break;
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderShader::CreatShader(const std::string& verShader, const std::string& fragShader) {
	program_id = glCreateProgram();

	vs_id = CompileShaderFile(GL_VERTEX_SHADER, verShader);
	//std::cout << "_______\n";
	fs_id = CompileShaderFile(GL_FRAGMENT_SHADER, fragShader);

	glAttachShader(program_id, vs_id);
	glAttachShader(program_id, fs_id);

	glLinkProgram(program_id);
	glValidateProgram(program_id);

	//glDeleteShader(vs_id);
	//glDeleteShader(fs_id);
	//glDeleteProgram(program_id);
}
////////////////////////////////////////////////////////
RenderShader::RenderShader(const std::string& name, const std::string& name2)
{
	vert_name = name;
	frag_name = name2 == "" ? name : name2;

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

GLuint RenderShader::getShaderID(ShaderType type) const
{
	switch (type)
	{
	case NONE_SHADER:
		return 0;
		break;
	case VERTEX_SHADER:
		return vs_id;
		break;
	case FRAGMENT_SHADER:
		return fs_id;
		break;
	default:
		return 0;
		break;
	}
}

void RenderShader::LocalDebug() const
{
#ifdef _DEBUG
	for (const auto i : _uniforms_cache)
		DEBUG(i.first + " : " + std::to_string(i.second))
#endif
}

ComputeShader::ComputeShader(const std::string& name)
	:comp_name(name)
{

	std::string file_name = name.find(ShaderLib::folder_root) == std::string::npos ? ShaderLib::folder_root + name + ShaderLib::file_type[COMPUTE_SHADER] : name;

	std::ifstream Stream(file_name);
	std::string Cache, Line;
	while (getline(Stream, Line))
		Cache += Line+"\n";

	CreateShader(Cache);
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

	comp_id = CompileShaderFile(GL_COMPUTE_SHADER, compShader);

	glAttachShader(program_id, comp_id);

	glLinkProgram(program_id);
}

GLuint ComputeShader::CompileShader()
{
	return comp_id;
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
