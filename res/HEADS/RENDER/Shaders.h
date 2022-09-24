#pragma once
#include "ShaderLib.h"
#include <functional>

enum ArrayType
{
	NULL_ARRAY, VEC1_ARRAY, VEC2_ARRAY, VEC3_ARRAY, VEC4_ARRAY, MAT4_ARRAY
};


class Shaders : public ShaderLib
{
private:
	ShaderPair m_shaders;
	GLuint program_id, vs_id, fs_id, cs_id=0;
	mutable std::unordered_map<std::string,GLuint> m_uniform_cache;
public:
	Shaders(const std::string& name, const std::string& name2 = "");
	Shaders();
	~Shaders();

	void ResetID(ShaderType type, GLuint id);
	void CreatShader(const std::string& verShader, const std::string& fragShader);
	void ParseShader(const std::string& name, const std::string& name2);
	GLuint CompileShader(ShaderType tar = NONE_SHADER);
	void ResetCache() { m_uniform_cache = {}; }
	void ResetUniform();

	void UseShader() const;
	void UnuseShader() const;

	void DelShad() const;

	inline GLuint getID() const { return program_id; }
	inline GLuint getShaderID(ShaderType type) const;
	inline GLuint getVarID(const char* name) const;

	std::function<void(void)> InitShader{};

	void SetValue(const std::string& name, const glm::mat4& projection);
	void SetValue(const std::string& name, const int& v0, const int& v1, const int& v2, const float& v3);
	void SetValue(const std::string& name, const int& v0, const int& v1, const int& v2);
	void SetValue(const std::string& name, const glm::vec3& vec3);
	void SetValue(const std::string& name, const glm::vec4& vec4);
	void SetValue(const std::string& name, const float& v0);
	void SetValue(const std::string& name, const int& v0);
	void SetValue(const std::string& name, const GLuint& v0);
	//void SetValue(const std::string& name, GLsizei count, const GLint* va0);
	void SetValue(const std::string& name, GLsizei count, const float* va0, ArrayType TYPE);
	void SetValue(const std::string& name, GLsizei count, const int* va0, ArrayType TYPE);

	void LocalDebug() const;
};

