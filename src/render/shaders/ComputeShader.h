#pragma once

#include "Shaders.h"
#include "RenderConfigs.h"

class ComputeShader : public Shaders {

private:

	using AvailUnis = std::variant<int, float, GLuint, Shaders::ArrayUni>;
	using Default = std::pair<std::string, AvailUnis>;

	static std::unordered_map<std::string, std::shared_ptr<ComputeShader>> comp_list;
	static std::unordered_map<std::string, std::vector<Default>> config_list;

	static void PushDefult(std::string name, std::string para_name, AvailUnis def);
	static void PushDefult(std::string name, std::string para_name, GLuint _size, float* _data, ArrayType _type);

public:

	ShaderUnit comp_shader;

public:

	ComputeShader(const std::string& name);
	template<class... Tuples>
	ComputeShader(const std::string& name, const Tuples&... args);
	ComputeShader();
	~ComputeShader();

	ComputeShader& operator=(ComputeShader&&) = default;
	ComputeShader& operator=(const ComputeShader&) = default;

	void ResetID(ShaderType tar, GLuint _id) override;
	void ResetDefult(std::string name);
	void CreateShader(const std::string& compShader);

	GLuint CompileShader(ShaderType tar = NONE_SHADER) override { return 0; };

	void ParseShaderCode(const std::string& _code, ShaderType tar) override;
	void RelinkShader(ShaderType tar = NONE_SHADER) override;
	void GenerateShader(ShaderType tar = NONE_SHADER) override {};

	ShaderUnit* GetShaderUnit(ShaderType tar = NONE_SHADER) override;

public:

	void RunComputeShaderSCR(const glm::vec2& _scr_size, GLuint _batch, bool _edge_fix = true);
	void RunComputeShader(const glm::vec2& _size);
	void RunComputeShader(GLuint workgroup_count_x = 1, GLuint workgroup_count_y = 1, GLuint workgroup_count_z = 1) const;

public:

	inline GLuint GetShaderID(ShaderType type) const override;
	std::vector<ShaderType> GetAllShaderTypes() const override { return { COMPUTE_SHADER }; };
	void LocalDebug() const override;

public:

	static ComputeShader& ImportShader(std::string _name);
	template<class... Tuples>
	static ComputeShader& ImportShader(std::string _name, const Tuples&... args);
	static std::shared_ptr<ComputeShader> ImportShaderSrc(std::string _name);
	template<class... Tuples>
	static std::shared_ptr<ComputeShader> ImportShaderSrc(std::string _name, const Tuples&... args);
	template<class... Tuples>
	static void ImportShaderConfigs(std::string _name, const Tuples&... args);

	static std::string GetSSRShaderName(RenderConfigs* config);
	static std::string GetAOShaderName(RenderConfigs* config);
	static std::string GetAAShaderName(RenderConfigs* config);
	static std::string GetShadowShaderName(char _type, char _light_type);

	static void InitComputeLib(RenderConfigs* config);
	static void ResetComputeLib();
};

template<class... Tuples>
ComputeShader::ComputeShader(const std::string& name, const Tuples&... args)
	:ComputeShader(name)
{
	UseShader();
	(std::apply([this](const auto&... args) { SetValue(args...); }, args), ...);
};

template<class... Tuples>
static std::shared_ptr<ComputeShader> ComputeShader::ImportShaderSrc(std::string _name, const Tuples&... args)
{
	if (comp_list.find(_name) != comp_list.end())
		return comp_list[_name];

	comp_list[_name] = std::make_shared<ComputeShader>(_name, args...);
	ImportShaderConfigs(_name, args...);
	return comp_list[_name];
}

template<class... Tuples>
void ComputeShader::ImportShaderConfigs(std::string _name, const Tuples&... args)
{
	(std::apply([_name](const auto&... def) { ComputeShader::PushDefult(_name, def...); }, args), ...);
}

template<class... Tuples>
ComputeShader& ComputeShader::ImportShader(std::string _name, const Tuples&... args)
{
	return *ImportShaderSrc(_name, args...).get();
}
