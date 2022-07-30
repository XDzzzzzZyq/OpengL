#pragma once
#include <GL/glew.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "structs.h"

enum ArrayType
{
	NULL_ARRAY, VEC1_ARRAY, VEC2_ARRAY, VEC3_ARRAY, VEC4_ARRAY, MAT4_ARRAY
};


class Shaders
{
private:
	ShaderPair m_shaders;
	GLuint program_id;
	mutable std::unordered_map<std::string,GLuint> m_ValLocCache;
public:
	Shaders(const std::string& path);
	Shaders();
	~Shaders();

	void UseShader() const;
	void UnuseShader() const;

	void DelShad() const;

	inline GLuint getID() const {	return program_id; }
	inline GLuint getVarID(const char* name) const;

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
};

