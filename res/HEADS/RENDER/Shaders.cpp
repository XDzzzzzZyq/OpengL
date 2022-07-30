#include "Shaders.h"
static ShaderPair ParseShader(const std::string& path) {
	std::ifstream stream(path);

	std::string line;

	int row = 0;

	enum class LineType
	{
		N = -1, V, F
	};
	LineType mode = LineType::N;

	std::stringstream shaders[2];

	while (getline(stream, line)) {
		if (line.find("shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				mode = LineType::V;
				//std::cout << "vertex" << std::endl;
			}
			else if (line.find("fragment") != std::string::npos) {
				mode = LineType::F;
				//std::cout << "frag" << std::endl;
			}

		}
		else {
			shaders[(int)mode] << line << "\n";
			//std::cout << row << std::endl;
		}
		//row++;
	}
	std::cout << "shaders are loaded up successfully!" << std::endl;
	return { shaders[0].str(),shaders[1].str() };
}

static GLuint CompileShader(GLuint TYPE, const std::string& source) {
	GLuint id = glCreateShader(TYPE);
	const char* src = source.c_str(); //传入指针，需要保证指向source（shader代码）的内存一直存在

	glShaderSource(id, 1, &src, nullptr);
	
	//std::cout << id << std::endl;
	glCompileShader(id);
	

	int status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	
	//std::cout << status << std::endl;
	if (!status) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "shader error" << std::endl;
		std::cout << message << std::endl;

		return 0;
	}
	else {
		std::string type = TYPE == GL_VERTEX_SHADER ? "Vertex" : "Frag";
		std::cout << type << " is complied successfully!" << std::endl;
	}

	return id;
}

static GLuint CreatShader(const std::string& verShader, const std::string& fragShader) {
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
	
	return program;
}
////////////////////////////////////////////////////////
Shaders::Shaders(const std::string& path)
{
	m_shaders = ParseShader(path);

	program_id = CreatShader(m_shaders.verShader, m_shaders.fragShader);
	//std::cout << "CreateShader:" << (glGetError()) << "\n";
	//std::cout << m_shaders.verShader << "\n";
}

Shaders::Shaders()
{

}

Shaders::~Shaders()
{
}

void Shaders::UseShader() const
{
	glUseProgram(program_id);
}

void Shaders::UnuseShader() const
{
	//glDeleteProgram(program_id);
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
	if (m_ValLocCache.find(name)!=m_ValLocCache.end())
		return m_ValLocCache[name];

	int id = glGetUniformLocation(program_id, name);
	if (id == -1)std::cout << name << " do not exist!" << std::endl;
	m_ValLocCache[name] = id;
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

void Shaders::SetValue(const std::string& name,const float& v0)
{
	int id = getVarID(name.c_str());
	glUniform1f(id, v0);
}

void Shaders::SetValue(const std::string& name,const int& v0)
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
		glUniform2fv(id, count*2, va0);
		break;
	case VEC3_ARRAY:
		glUniform3fv(id, count*3, va0);
		break;
	case VEC4_ARRAY:
		glUniform4fv(id, count*4, va0);
		break;
	case MAT4_ARRAY:
		glUniform4fv(id, count*4, va0);
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

