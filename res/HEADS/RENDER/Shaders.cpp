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
	std::string type = TYPE == GL_VERTEX_SHADER ? "Vertex" : "Frag";
	if (!status) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];

		glGetShaderInfoLog(id, length, &length, message);
		std::cout << type + " shader error" << std::endl;
		std::cout << message << std::endl;
		//delete message;
		return 0;
	}
#ifdef _DEBUG
	std::cout << type << " is complied successfully!" << std::endl;
#endif
	//delete src;
	return id;
}

void Shaders::CreatShader(const std::string& verShader, const std::string& fragShader) {
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
Shaders::Shaders(const std::string& name, const std::string& name2)
{
	vert_name = name;
	frag_name = name2 == "" ? name : name2;

	ParseShaderFile(vert_name, VERTEX_SHADER);
	ParseShaderFile(frag_name, FRAGMENT_SHADER);
	GenerateShader();
	CreatShader(shader_list[VERTEX_SHADER], shader_list[FRAGMENT_SHADER]);
}

Shaders::Shaders()
{
}

Shaders::~Shaders()
{
}

void Shaders::ResetID(ShaderType type, GLuint id)
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
	case COMPUTE_SHADER:
		cs_id = id;
		break;
	default:
		break;
	}
}

void Shaders::ParseShaderCode(const std::string& _code, ShaderType _type)
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

GLuint Shaders::CompileShader(ShaderType tar)
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
		std::cout << type + " shader error" << std::endl;
		std::cout << message << std::endl;
	}
	else {
		std::cout << type << " is complied successfully!" << std::endl;
		shader_struct_list[tar].is_struct_changed = false;
	}
	tar == 0 ? vs_id = shader_id : fs_id = shader_id;
	return shader_id;
}

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

GLuint Shaders::getShaderID(ShaderType type) const
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
	case COMPUTE_SHADER:
		return cs_id;
		break;
	default:
		return 0;
		break;
	}
}

GLuint Shaders::getVarID(const char* name) const
{
	this->UseShader();
	//std::cout << program_id << "\n";
	if (m_uniform_cache.find(name) != m_uniform_cache.end())
		return m_uniform_cache[name];

	int id = glGetUniformLocation(program_id, name);
	if (id == -1)std::cout << name << " do not exist!" << std::endl;
	m_uniform_cache[name] = id;
	return glGetUniformLocation(program_id, name);
}

void Shaders::SetValue(const std::string& name, const int& v0, const int& v1, const int& v2, const float& v3)
{
	int id = getVarID(name.c_str());
	glUniform4f(id, v0, v1, v2, v3);
}

void Shaders::SetValue(const std::string& name, const int& v0, const int& v1, const int& v2)
{
	int id = getVarID(name.c_str());
	glUniform3f(id, v0, v1, v2);
}

void Shaders::SetValue(const std::string& name, const glm::mat4& projection)
{
	int id = getVarID(name.c_str());
	glUniformMatrix4fv(id, 1, GL_FALSE, &projection[0][0]);
}

void Shaders::SetValue(const std::string& name, const float& v0)
{
	int id = getVarID(name.c_str());
	glUniform1f(id, v0);
}

void Shaders::SetValue(const std::string& name, const int& v0)
{
	int id = getVarID(name.c_str());
	glUniform1i(id, v0);
}

void Shaders::SetValue(const std::string& name, const GLuint& v0)
{
	int id = getVarID(name.c_str());
	glUniform1i(id, v0);
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

void Shaders::LocalDebug() const
{
#ifdef _DEBUG
	for (const auto i : m_uniform_cache)
		DEBUG(i.first + " : " + std::to_string(i.second))
#endif
}

