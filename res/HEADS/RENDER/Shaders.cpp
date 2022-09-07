#include "Shaders.h"
GLuint CompileShader(GLuint TYPE, const std::string& source) {
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
	else {
		std::cout << type << " is complied successfully!" << std::endl;
	}
	//delete src;
	return id;
}

GLuint CreatShader(const std::string& verShader, const std::string& fragShader) {
	GLuint program = glCreateProgram();

	GLuint vs = CompileShader(GL_VERTEX_SHADER, verShader);
	//std::cout << "_______\n";
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	//std::cout << program << " " << vs << " " << fs << " \n";

	return program;
}
////////////////////////////////////////////////////////
Shaders::Shaders(const std::string& name, const std::string& name2)
{
	vert_name = name;
	frag_name = name2 == "" ? name : name2;

	ParseShader(vert_name, frag_name);
	GenerateShader();
	program_id = CreatShader(shader_list[VERTEX_SHADER], shader_list[FRAGMENT_SHADER]);
}

Shaders::Shaders()
{
	DEBUG("def sh")

}

Shaders::~Shaders()
{
}

std::string Shaders::folder_root = "res/shaders/";;
std::vector<std::string> Shaders::file_type = { ".vert",".frag" };

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
	for (const auto i : m_uniform_cache)
		DEBUG(i.first + " : " + std::to_string(i.second))
}

