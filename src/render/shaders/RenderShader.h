/**
 * @file RenderShader.h
 * @brief Render shader implementation for vertex and fragment shader pipelines.
 * 
 * RenderShader manages traditional rasterization pipelines with vertex and fragment stages.
 * It handles shader loading, compilation, linking, and provides optimized variants for
 * fast loading and multi-stage chaining.
 * 
 * GPU Resource Ownership:
 * - Owns vertex and fragment shader objects
 * - Owns linked shader program
 * - Resources released via glDeleteShader/glDeleteProgram in destructor
 * 
 * Architecture:
 * - RenderShader: Full-featured shader with runtime recompilation
 * - FastLoadShader: Optimized for loading pre-compiled shaders (WIP)
 * - ChainedShader: Supports vertex, fragment, and optional geometry shaders
 */

#pragma once
#include "Shaders.h"
#include <array>

/**
 * @brief Standard render shader with vertex and fragment stages.
 * 
 * RenderShader is the primary shader type for rasterization rendering. It compiles
 * vertex and fragment shader code, links them into a program, and provides runtime
 * recompilation support for live shader editing.
 * 
 * Usage:
 * @code
 * RenderShader shader("pbr_vert", "pbr_frag");  // Load from files
 * shader.UseShader();
 * shader.SetValue("u_projection", camera.GetProjection());
 * // ... draw calls ...
 * shader.UnuseShader();
 * @endcode
 * 
 * @note Shader files are loaded from Shaders::folder_root with appropriate extensions.
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 */
class RenderShader : public Shaders
{
private:

	std::array<ShaderUnit, 2> shader_data; ///< Array of [vertex, fragment] shader units

public:

	/**
	 * @brief Constructs and loads a render shader from files.
	 * @param vert Vertex shader file name (without extension)
	 * @param frag Fragment shader file name (without extension, defaults to vert name)
	 * @note Automatically loads and compiles shaders from disk
	 */
	RenderShader(const std::string& vert, const std::string& frag = "");
	
	/**
	 * @brief Default constructor (creates empty shader).
	 */
	RenderShader();
	
	/**
	 * @brief Destructor (releases vertex and fragment shader objects).
	 */
	~RenderShader();

	/**
	 * @brief Move assignment operator.
	 * @param other Shader to move from
	 * @return Reference to this shader
	 */
	RenderShader& operator=(RenderShader&&) = default;

	/**
	 * @brief Creates shader program from vertex and fragment shader names.
	 * @param verShader Vertex shader file name
	 * @param fragShader Fragment shader file name
	 * @note Loads files, compiles shaders, and links program
	 */
	void CreatShader(const std::string& verShader, const std::string& fragShader);

public:

	/**
	 * @brief Parses shader code from an input stream.
	 * @param _stream Input stream containing GLSL code
	 * @param _type Shader stage type (vertex or fragment)
	 */
	void ParseShaderStream(std::istream& _stream, ShaderType _type);
	
	/**
	 * @brief Parses shader code from a file.
	 * @param _name Shader file name (without path)
	 * @param _type Shader stage type (vertex or fragment)
	 */
	void ParseShaderFile(std::string _name, ShaderType _type);

	/**
	 * @brief Parses raw GLSL code string.
	 * @param _code GLSL source code
	 * @param _type Shader stage type (vertex or fragment)
	 */
	void ParseShaderCode(const std::string& _code, ShaderType _type) override;
	
	/**
	 * @brief Resets shader ID for a specific stage.
	 * @param type Shader stage type
	 * @param id New OpenGL shader object ID
	 */
	void ResetID(ShaderType type, GLuint id) override;
	
	/**
	 * @brief Relinks the shader program.
	 * @param tar Target shader to recompile before linking, or NONE_SHADER to link without recompilation
	 */
	void RelinkShader(ShaderType tar = NONE_SHADER) override;
	
	/**
	 * @brief Generates shader code (unused for standard shaders).
	 * @param tar Target shader stage
	 */
	void GenerateShader(ShaderType tar = NONE_SHADER) override;
	
	/**
	 * @brief Returns pointer to shader unit for a specific stage.
	 * @param tar Shader stage (vertex or fragment)
	 * @return Pointer to ShaderUnit
	 */
	ShaderUnit* GetShaderUnit(ShaderType tar = NONE_SHADER) override;
	
	/**
	 * @brief Compiles a shader stage.
	 * @param tar Shader stage to compile, or NONE_SHADER to compile both
	 * @return OpenGL shader object ID, or 0 on failure
	 */
	GLuint CompileShader(ShaderType tar = NONE_SHADER) override;

public:

	/**
	 * @brief Returns OpenGL shader ID for a specific stage.
	 * @param type Shader stage type
	 * @return Shader object ID
	 */
	inline GLuint GetShaderID(ShaderType type) const override;
	
	/**
	 * @brief Returns list of shader stages in this program.
	 * @return Vector containing VERTEX_SHADER and FRAGMENT_SHADER
	 */
	std::vector<ShaderType> GetAllShaderTypes() const override { return { VERTEX_SHADER, FRAGMENT_SHADER }; };
	
	/**
	 * @brief Prints debug information about vertex and fragment shaders.
	 */
	void LocalDebug() const override;

};



/**
 * @brief Fast-loading shader variant for pre-compiled shaders.
 * 
 * FastLoadShader is optimized for quick shader loading by bypassing some
 * features like runtime recompilation and structure parsing. Intended for
 * production builds where shader editing is not needed.
 * 
 * @note Currently WIP: May not support all features of RenderShader.
 */
class FastLoadShader : public Shaders
{
private:

	std::string vert_name, frag_name; ///< Shader file names
	ShaderPair fast_shaders;          ///< Loaded shader code
	GLuint vs_id{ 0 }, fs_id{ 0 };    ///< Vertex and fragment shader object IDs

public:

	/**
	 * @brief Constructs and loads a fast shader from files.
	 * @param vert Vertex shader file name
	 * @param frag Fragment shader file name (defaults to vert name)
	 */
	FastLoadShader(const std::string& vert, const std::string& frag = "");
	
	/**
	 * @brief Default constructor.
	 */
	FastLoadShader();
	
	/**
	 * @brief Destructor (releases shader objects).
	 */
	~FastLoadShader();

	/**
	 * @brief Move assignment operator.
	 */
	FastLoadShader& operator=(FastLoadShader&&) = default;

	/**
	 * @brief Creates shader program from vertex and fragment shader names.
	 * @param verShader Vertex shader file name
	 * @param fragShader Fragment shader file name
	 */
	void CreatShader(const std::string& verShader, const std::string& fragShader);
	
	/**
	 * @brief Resets shader ID (not supported in FastLoadShader).
	 */
	void ResetID(ShaderType type, GLuint id) override {};
	
	/**
	 * @brief Relinks shader (not supported in FastLoadShader).
	 */
	void RelinkShader(ShaderType tar = NONE_SHADER) override {};
	
	/**
	 * @brief Generates shader (not supported in FastLoadShader).
	 */
	void GenerateShader(ShaderType tar = NONE_SHADER) override {};

public:

	/**
	 * @brief Returns OpenGL shader ID for a specific stage.
	 * @param type Shader stage type
	 * @return Shader object ID
	 */
	inline GLuint GetShaderID(ShaderType type) const override;
	
	/**
	 * @brief Returns list of shader stages.
	 * @return Vector containing VERTEX_SHADER and FRAGMENT_SHADER
	 */
	std::vector<ShaderType> GetAllShaderTypes() const override { return { VERTEX_SHADER, FRAGMENT_SHADER }; };
	
	/**
	 * @brief Prints debug information.
	 */
	void LocalDebug() const override;

};



/**
 * @brief Multi-stage shader supporting vertex, fragment, and optional geometry stages.
 * 
 * ChainedShader allows flexible shader pipelines with arbitrary combinations of
 * vertex, fragment, and geometry shaders. Supports runtime recompilation and
 * shader hot-reloading. Shaders are cached globally by name.
 * 
 * Usage:
 * @code
 * // Create shader with vertex, geometry, and fragment stages
 * auto& shader = ChainedShader::ImportShader({"vert_name", "geom_name", "frag_name"});
 * 
 * // Or using variadic template
 * auto& shader2 = ChainedShader::ImportShader("vert_name", "frag_name");
 * @endcode
 * 
 * @note Shaders are cached in chain_sh_list and shared across instances.
 */
class ChainedShader : public Shaders
{
private:
	static std::unordered_map<std::string, std::shared_ptr<ChainedShader>> chain_sh_list; ///< Global shader cache

private:

	using ShaderChain = std::vector<ShaderUnit>;
	ShaderChain shader_chain; ///< Vector of shader units in pipeline order

	std::array<int, 4> _type_to_idx{ -1, -1, -1, -1 }; ///< Maps ShaderType enum to index in shader_chain
	std::vector<ShaderType> _idx_to_type;              ///< Maps index to ShaderType enum

public:

	/**
	 * @brief Constructs a chained shader from a list of shader file names.
	 * @param chain Vector of shader file names (order determines pipeline stages)
	 * @note File extensions determine shader stages (.vert, .frag, .geom)
	 */
	ChainedShader(const std::vector<std::string>& chain);
	
	/**
	 * @brief Default constructor.
	 */
	ChainedShader();
	
	/**
	 * @brief Destructor (releases all shader objects in chain).
	 */
	~ChainedShader();

	/**
	 * @brief Move assignment operator.
	 */
	ChainedShader& operator=(ChainedShader&&) = default;
	
	/**
	 * @brief Copy assignment operator.
	 */
	ChainedShader& operator=(const ChainedShader&) = default;

	/**
	 * @brief Creates shader program from loaded shader chain.
	 * @note Compiles all shader units and links them into a program
	 */
	void CreatShader();
	
	/**
	 * @brief Imports or retrieves a cached chained shader.
	 * @param chain Vector of shader file names
	 * @return Reference to cached ChainedShader instance
	 * @note If shader already exists in cache, returns existing instance
	 */
	static ChainedShader& ImportShader(const std::vector<std::string>& chain);
	
	/**
	 * @brief Imports or retrieves a cached chained shader (variadic version).
	 * @tparam _Name Variadic template for shader names
	 * @param name Shader file names
	 * @return Reference to cached ChainedShader instance
	 */
	template<class... _Name>
	static ChainedShader& ImportShader(_Name ...name);

	/**
	 * @brief Resets shader ID for a specific stage.
	 * @param type Shader stage type
	 * @param id New OpenGL shader object ID
	 */
	void ResetID(ShaderType type, GLuint id) override;
	
	/**
	 * @brief Relinks the shader program.
	 * @param type Shader stage to recompile before linking, or NONE_SHADER to link all
	 */
	void RelinkShader(ShaderType type = NONE_SHADER) override;

	/**
	 * @brief Parses raw GLSL code for a specific stage.
	 * @param _code GLSL source code
	 * @param type Shader stage type
	 */
	void ParseShaderCode(const std::string& _code, ShaderType type) override;
	
	/**
	 * @brief Generates shader code (unused for ChainedShader).
	 */
	void GenerateShader(ShaderType type = NONE_SHADER) override { return; };
	
	/**
	 * @brief Returns pointer to shader unit for a specific stage.
	 * @param type Shader stage type
	 * @return Pointer to ShaderUnit, or nullptr if stage not in chain
	 */
	ShaderUnit* GetShaderUnit(ShaderType type = NONE_SHADER) override;
	
	/**
	 * @brief Compiles a shader stage.
	 * @param type Shader stage to compile
	 * @return OpenGL shader object ID, or 0 on failure
	 */
	GLuint CompileShader(ShaderType type = NONE_SHADER) override;

public:

	/**
	 * @brief Returns OpenGL shader ID for a specific stage.
	 * @param type Shader stage type
	 * @return Shader object ID, or 0 if stage not in chain
	 */
	inline GLuint GetShaderID(ShaderType type) const override;
	
	/**
	 * @brief Returns list of all shader stages in this chain.
	 * @return Vector of ShaderType enums
	 */
	std::vector<ShaderType> GetAllShaderTypes() const override { return _idx_to_type; };
	
	/**
	 * @brief Prints debug information about all shaders in chain.
	 */
	void LocalDebug() const override;
};

template<class... _Name>
ChainedShader& ChainedShader::ImportShader(_Name ...name)
{
	return ChainedShader::ImportShader(std::vector<std::string>{ name... });
}