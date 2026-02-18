#pragma once

#include "Shaders.h"

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
