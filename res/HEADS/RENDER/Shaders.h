#pragma once

#include "ShaderLib.h"
#include <functional>

enum ArrayType
{
	NULL_ARRAY, VEC1_ARRAY, VEC2_ARRAY, VEC3_ARRAY, VEC4_ARRAY, MAT4_ARRAY
};

#define Uni std::make_tuple

class Shaders {

public:

	using ShaderConstInfo = std::tuple<std::string, std::string, GLuint>; // name | filename | GL_name

	static ShaderConstInfo ParseShaderType(ShaderType _type);
	static ShaderType ParseFileEXT(std::string path);

	static GLuint CompileShaderCode(ShaderType _type, const std::string& source);
	static std::string ReadShaderFile(ShaderType _type, const std::string& name);

protected:

	GLuint program_id;
	mutable std::unordered_map<std::string, GLuint> _uniforms_cache;

public:

	inline GLuint getProgramID() const { return program_id; }
	virtual GLuint getShaderID(ShaderType type) const = 0;

public:

	std::function<void(void)> InitShader{};
	void ResetCache() { _uniforms_cache = {}; }

	void UseShader() const;
	void UnuseShader() const;
	void DelShad();

public:

	inline GLuint getVarID(const char* name) const;

	void SetValue(const std::string& name, const glm::mat4& projection);
	void SetValue(const std::string& name, int v0, int v1, int v2, float v3);
	void SetValue(const std::string& name, int v0, int v1, int v2);
	void SetValue(const std::string& name, const glm::vec2& vec2);
	void SetValue(const std::string& name, const glm::vec3& vec3);
	void SetValue(const std::string& name, const glm::vec4& vec4);
	void SetValue(const std::string& name, float v0);
	void SetValue(const std::string& name, int v0);
	void SetValue(const std::string& name, bool v0);
	void SetValue(const std::string& name, const GLuint& v0);
	//void SetValue(const std::string& name, GLsizei count, const GLint* va0);
	void SetValue(const std::string& name, GLsizei count, const float* va0, ArrayType TYPE);
	void SetValue(const std::string& name, GLsizei count, const int* va0, ArrayType TYPE);
	void SetValue(const std::string& name, GLsizei count, const GLuint* va0, ArrayType TYPE);
	void SetValue(const std::string& name, GLsizei count, const glm::mat4* va0);

public:

	virtual void LocalDebug() const = 0;

};


class RenderShader : public Shaders, public ShaderLib
{
private:

	std::string vert_name, frag_name;
	ShaderPair rend_shaders;
	GLuint vs_id{ 0 }, fs_id{ 0 };

public:

	RenderShader(const std::string& vert, const std::string& frag = "");
	RenderShader();
	~RenderShader();

	void ResetID(ShaderType type, GLuint id);
	void CreatShader(const std::string& verShader, const std::string& fragShader);

public:

	void ParseShaderStream(std::istream& _stream, ShaderType _type);
	void ParseShaderFile(std::string _name, ShaderType _type);
	void ParseShaderCode(const std::string& _code, ShaderType _type);
	GLuint CompileShader(ShaderType tar = NONE_SHADER) override;

public:

	inline GLuint getShaderID(ShaderType type) const override;
	void LocalDebug() const override;

};



class FastLoadShader : public Shaders
{
private:

	std::string vert_name, frag_name;
	ShaderPair fast_shaders;
	GLuint vs_id{ 0 }, fs_id{ 0 };

public:

	FastLoadShader(const std::string& vert, const std::string& frag = "");
	FastLoadShader();
	~FastLoadShader();

	void CreatShader(const std::string& verShader, const std::string& fragShader);

public:

	inline GLuint getShaderID(ShaderType type) const override;
	void LocalDebug() const override;

};



class ChainedShader : public Shaders
{
private:

	struct ShaderNode 
	{
		ShaderType type;
		std::string name;
		GLuint ID = 0;
	};
	
	using ShaderChain = std::vector<ShaderNode>;

	ShaderChain shader_chain;

public:

	ChainedShader(const std::vector<std::string>& chain);
	ChainedShader();
	~ChainedShader();

	void CreatShader();

public:

	inline GLuint getShaderID(ShaderType type) const override;
	void LocalDebug() const override;
};



class ComputeShader : public Shaders {

private:
	static std::unordered_map<std::string, std::shared_ptr<ComputeShader>> comp_list;

public:

	std::string comp_name;
	GLuint comp_id;
	static void ResetComputeLib();

public:

	ComputeShader(const std::string& name);
	template<class... Tuples>
	ComputeShader(const std::string& name, const Tuples&... args);
	ComputeShader();
	~ComputeShader();

	void ResetID(GLuint _id) { comp_id = _id; }
	void CreateShader(const std::string& compShader);
	GLuint CompileShader();

public:

	void RunComputeShader(const ImVec2& _size);
	void RunComputeShader(GLuint workgroup_count_x = 1, GLuint workgroup_count_y = 1, GLuint workgroup_count_z = 1) const;

public:

	inline GLuint getShaderID(ShaderType type) const override;
	void LocalDebug() const override;

public:

	static ComputeShader& ImportShader(std::string _name);
	template<class... Tuples>
	static ComputeShader& ImportShader(std::string _name, const Tuples&... args);

};

template<class... Tuples>
ComputeShader::ComputeShader(const std::string& name, const Tuples&... args)
	:ComputeShader(name)
{
	UseShader();
	(std::apply([this](const auto&... args) { SetValue(args...); }, args), ...);
};

template<class... Tuples>
ComputeShader& ComputeShader::ImportShader(std::string _name, const Tuples&... args)
{
	if (comp_list.find(_name) != comp_list.end())
		return *comp_list[_name].get();

	comp_list[_name] = std::make_shared<ComputeShader>(_name, args...);
	return *comp_list[_name].get();
}
