#pragma once

#include "ShaderLib.h"
#include "RenderConfigs.h"

#include <functional>
#include <optional>
#include <variant>
#include <array>

enum ArrayType
{
	NULL_ARRAY, VEC1_ARRAY, VEC2_ARRAY, VEC3_ARRAY, VEC4_ARRAY, MAT4_ARRAY
};

enum ShaderType
{
	NONE_SHADER = -1, VERTEX_SHADER, FRAGMENT_SHADER, COMPUTE_SHADER, GEOMETRY_SHADER
};

#define Uni std::make_tuple

class Shaders {

public:

	static std::string const shader_type[4];

	struct ShaderPair {
		std::string verShader;
		std::string fragShader;
	};

	class ShaderUnit
	{
	private:
		void _del();
		void _copyInfo(const ShaderUnit& unit);
		void _resetID(GLuint ID);
	public:
		ShaderType sh_type{ NONE_SHADER };
		std::string sh_name{ "" };
		GLuint sh_ID{ 0 };
		std::string sh_code{ "" };
		std::optional<ShaderStruct> sh_struct{};

		ShaderUnit() {}
		ShaderUnit(ShaderType type, std::string name);
		ShaderUnit(const ShaderUnit& unit);
		ShaderUnit(ShaderUnit&& unit) noexcept;
		~ShaderUnit();

		ShaderUnit& operator=(const ShaderUnit& unit);
		ShaderUnit& operator=(ShaderUnit&& unit) noexcept;

		struct hash_fn
		{
			std::size_t operator()(const ShaderUnit& inp) const;
		};
	};

	struct ArrayUni 
	{
		GLuint size;
		float* data;
		ArrayType type;
	};

public:

	Shaders() {}
	Shaders(const Shaders& shader);
	Shaders(Shaders&& shader) noexcept;
	~Shaders();

	Shaders& operator=(const Shaders& shader);
	Shaders& operator=(Shaders&& shader) noexcept;

protected:

	void _del();
	void _resetProgramID(GLuint _ID) { if (program_id > 0 && program_id != _ID)_del(); program_id = _ID; }

public:

	using ShaderConstInfo = std::tuple<std::string, std::string, GLuint>; // name | filename | GL_name

	static ShaderConstInfo ParseShaderType(ShaderType _type);
	static ShaderType ParseFileEXT(std::string path);

	static GLuint CompileShaderCode(ShaderType _type, const std::string& source);
	static std::string ReadShaderFile(ShaderType _type, const std::string& name);

	static std::string folder_root;
	static std::vector<std::string> file_type;

public:

	GLuint program_id{ 0 };

protected:

	mutable std::unordered_map<std::string, GLuint> _uniforms_cache;
	std::unordered_map<std::string, int> _LINK_LOC;

	bool _is_link_repeat(const std::string _name) { for (auto& i : _LINK_LOC) if (_name == i.first)return true; return false; }

public:

	bool is_shader_changed{ true };
	inline GLuint GetProgramID() const { return program_id; }
	virtual GLuint GetShaderID(ShaderType type) const = 0;
	virtual std::vector<ShaderType> GetAllShaderTypes() const = 0;

	virtual void ResetID(ShaderType type, GLuint id) = 0;
	virtual void RelinkShader(ShaderType tar = NONE_SHADER) = 0;

	virtual void ParseShaderCode(const std::string& _code, ShaderType _type) = 0;
	virtual void GenerateShader(ShaderType tar = NONE_SHADER) = 0;
	virtual ShaderUnit* GetShaderUnit(ShaderType tar = NONE_SHADER) = 0;
	virtual GLuint CompileShader(ShaderType tar) = 0;

public:

	std::function<void(void)> InitShader{};
	void ResetCache() { _uniforms_cache = {}; }

	void UseShader() const;
	void UnuseShader() const;

public:

	inline GLuint getVarID(const char* name) const;

	void SetValue(const std::string& name, const glm::mat4& projection);
	void SetValue(const std::string& name, int v0);
	void SetValue(const std::string& name, int v0, int v1, int v2);
	void SetValue(const std::string& name, int v0, int v1, int v2, int v3);
	void SetValue(const std::string& name, float v0);
	void SetValue(const std::string& name, float v0, float v1);
	void SetValue(const std::string& name, float v0, float v1, float v2);
	void SetValue(const std::string& name, float v0, float v1, float v2, float v3);
	void SetValue(const std::string& name, const glm::vec2& vec2);
	void SetValue(const std::string& name, const glm::vec3& vec3);
	void SetValue(const std::string& name, const glm::vec4& vec4);
	void SetValue(const std::string& name, bool v0);
	void SetValue(const std::string& name, const GLuint& v0);
	//void SetValue(const std::string& name, GLsizei count, const GLint* va0);
	void SetValue(const std::string& name, GLsizei count, const float* va0, ArrayType TYPE);
	void SetValue(const std::string& name, Shaders::ArrayUni arr);
	void SetValue(const std::string& name, GLsizei count, const int* va0, ArrayType TYPE);
	void SetValue(const std::string& name, GLsizei count, const GLuint* va0, ArrayType TYPE);
	void SetValue(const std::string& name, GLsizei count, const glm::mat4* va0);

public:

	virtual void LocalDebug() const = 0;

};


// Forward declaration - RenderShader is now in shaders/RenderShader.h
class RenderShader;



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

	std::array<int, 4> _type_to_idx {-1, -1, -1, -1};
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


// Forward declaration - ComputeShader is now in shaders/ComputeShader.h
class ComputeShader;
