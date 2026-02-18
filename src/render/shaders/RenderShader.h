#pragma once
#include "Shaders.h"
#include <array>

class RenderShader : public Shaders
{
private:

	std::array<ShaderUnit, 2> shader_data;

public:

	RenderShader(const std::string& vert, const std::string& frag = "");
	RenderShader();
	~RenderShader();

	RenderShader& operator=(RenderShader&&) = default;

	void CreatShader(const std::string& verShader, const std::string& fragShader);

public:

	void ParseShaderStream(std::istream& _stream, ShaderType _type);
	void ParseShaderFile(std::string _name, ShaderType _type);

	void ParseShaderCode(const std::string& _code, ShaderType _type) override;
	void ResetID(ShaderType type, GLuint id) override;
	void RelinkShader(ShaderType tar = NONE_SHADER) override;
	void GenerateShader(ShaderType tar = NONE_SHADER) override;
	ShaderUnit* GetShaderUnit(ShaderType tar = NONE_SHADER) override;
	GLuint CompileShader(ShaderType tar = NONE_SHADER) override;

public:

	inline GLuint GetShaderID(ShaderType type) const override;
	std::vector<ShaderType> GetAllShaderTypes() const override { return { VERTEX_SHADER, FRAGMENT_SHADER }; };
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

	FastLoadShader& operator=(FastLoadShader&&) = default;

	void CreatShader(const std::string& verShader, const std::string& fragShader);
	void ResetID(ShaderType type, GLuint id) override {};
	void RelinkShader(ShaderType tar = NONE_SHADER) override {};
	void GenerateShader(ShaderType tar = NONE_SHADER) override {};

public:

	inline GLuint GetShaderID(ShaderType type) const override;
	std::vector<ShaderType> GetAllShaderTypes() const override { return { VERTEX_SHADER, FRAGMENT_SHADER }; };
	void LocalDebug() const override;

};



class ChainedShader : public Shaders
{
private:
	static std::unordered_map<std::string, std::shared_ptr<ChainedShader>> chain_sh_list;
private:

	using ShaderChain = std::vector<ShaderUnit>;
	ShaderChain shader_chain;

	std::array<int, 4> _type_to_idx{ -1, -1, -1, -1 };
	std::vector<ShaderType> _idx_to_type;

public:

	ChainedShader(const std::vector<std::string>& chain);
	ChainedShader();
	~ChainedShader();

	ChainedShader& operator=(ChainedShader&&) = default;
	ChainedShader& operator=(const ChainedShader&) = default;

	void CreatShader();
	static ChainedShader& ImportShader(const std::vector<std::string>& chain);
	template<class... _Name>
	static ChainedShader& ImportShader(_Name ...name);

	void ResetID(ShaderType type, GLuint id) override;
	void RelinkShader(ShaderType type = NONE_SHADER) override;

	void ParseShaderCode(const std::string& _code, ShaderType type) override;
	void GenerateShader(ShaderType type = NONE_SHADER) override { return; };
	ShaderUnit* GetShaderUnit(ShaderType type = NONE_SHADER) override;
	GLuint CompileShader(ShaderType type = NONE_SHADER) override;

public:

	inline GLuint GetShaderID(ShaderType type) const override;
	std::vector<ShaderType> GetAllShaderTypes() const override { return _idx_to_type; };
	void LocalDebug() const override;
};

template<class... _Name>
ChainedShader& ChainedShader::ImportShader(_Name ...name)
{
	return ChainedShader::ImportShader(std::vector<std::string>{ name... });
}