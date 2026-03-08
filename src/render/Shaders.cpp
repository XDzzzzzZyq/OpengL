#include "Shaders.h"
#include "ShaderIO.h"

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
		printf(source.c_str());
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

	return ShaderIO::Read(file_name);
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
