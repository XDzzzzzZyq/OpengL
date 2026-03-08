/**
 * @file ComputeShader.h
 * @brief Compute shader implementation for GPGPU operations and post-processing.
 * 
 * ComputeShader manages OpenGL compute shaders for GPU-accelerated parallel computation.
 * Used extensively in the rendering pipeline for post-processing effects (SSAO, SSR, FXAA),
 * shadow generation, and other GPGPU tasks. Supports caching and default parameter configs.
 * 
 * GPU Resource Ownership:
 * - Owns compute shader object
 * - Owns linked shader program
 * - Resources released via glDeleteShader/glDeleteProgram in destructor
 * 
 * Architecture:
 * - Shader cache managed by AssetManager for reuse across renderer
 * - Config cache (config_list) for storing default uniform values
 * - Runtime dispatch with configurable workgroup sizes
 */

#pragma once
#include "Shaders.h"
#include "RenderConfigs.h"
#include "AssetManager.h"

/**
 * @brief Compute shader for GPGPU parallel computation.
 * 
 * ComputeShader provides GPU compute capabilities for tasks like image processing,
 * post-processing effects, physics simulation, and data transformation. Supports
 * global caching for efficient reuse and default uniform configurations.
 * 
 * Usage:
 * @code
 * // Import cached shader with default uniforms
 * auto& shader = ComputeShader::ImportShader("ssao_comp", 
 *     Uni("u_radius", 0.5f), 
 *     Uni("u_samples", 32));
 * 
 * // Bind textures, set uniforms, and dispatch
 * shader.UseShader();
 * shader.SetValue("u_input", 0);
 * shader.RunComputeShaderSCR(viewport_size, 16);  // 16x16 workgroups
 * shader.UnuseShader();
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 * @note Workgroup size: Default 16x16, configurable via local_size_x/y/z in GLSL
 */
class ComputeShader : public Shaders {

private:

	using AvailUnis = std::variant<int, float, GLuint, Shaders::ArrayUni>; ///< Supported uniform types
	using Default = std::pair<std::string, AvailUnis>;                     ///< Default uniform: (name, value)

	static std::unordered_map<std::string, std::vector<Default>> config_list;         ///< Default uniform configurations

	/**
	 * @brief Adds a default uniform value to a shader's config.
	 * @param name Shader name
	 * @param para_name Uniform parameter name
	 * @param def Default value (int, float, GLuint, or ArrayUni)
	 */
	static void PushDefult(std::string name, std::string para_name, AvailUnis def);
	
	/**
	 * @brief Adds a default array uniform to a shader's config.
	 * @param name Shader name
	 * @param para_name Uniform parameter name
	 * @param _size Array size
	 * @param _data Array data pointer
	 * @param _type Array element type
	 */
	static void PushDefult(std::string name, std::string para_name, GLuint _size, float* _data, ArrayType _type);

public:

	ShaderUnit comp_shader; ///< Compute shader unit

public:

	/**
	 * @brief Constructs a compute shader from file.
	 * @param name Compute shader file name (without extension)
	 */
	ComputeShader(const std::string& name);
	
	/**
	 * @brief Constructs a compute shader with default uniform values.
	 * @tparam Tuples Variadic template for uniform tuples (name, value)
	 * @param name Compute shader file name
	 * @param args Uniform tuples created with Uni() macro
	 * @note Automatically sets uniforms after construction
	 */
	template<class... Tuples>
	ComputeShader(const std::string& name, const Tuples&... args);
	
	/**
	 * @brief Default constructor (creates empty shader).
	 */
	ComputeShader();
	
	/**
	 * @brief Destructor (releases compute shader object).
	 */
	~ComputeShader();

	/**
	 * @brief Move assignment operator.
	 */
	ComputeShader& operator=(ComputeShader&&) = default;
	
	/**
	 * @brief Copy assignment operator.
	 */
	ComputeShader& operator=(const ComputeShader&) = default;

	/**
	 * @brief Resets compute shader ID.
	 * @param tar Shader type (must be COMPUTE_SHADER)
	 * @param _id New OpenGL shader object ID
	 */
	void ResetID(ShaderType tar, GLuint _id) override;
	
	/**
	 * @brief Resets shader uniforms to default values from config.
	 * @param name Shader name (must match config_list entry)
	 */
	void ResetDefult(std::string name);
	
	/**
	 * @brief Creates compute shader program from file.
	 * @param compShader Compute shader file name
	 * @note Loads, compiles, and links the shader
	 */
	void CreateShader(const std::string& compShader);

	/**
	 * @brief Compiles the compute shader (unused in ComputeShader).
	 * @return Always returns 0
	 */
	GLuint CompileShader(ShaderType tar = NONE_SHADER) override { return 0; };

	/**
	 * @brief Parses raw GLSL code for compute shader.
	 * @param _code GLSL compute shader source code
	 * @param tar Shader type (must be COMPUTE_SHADER)
	 */
	void ParseShaderCode(const std::string& _code, ShaderType tar) override;
	
	/**
	 * @brief Relinks the compute shader program.
	 * @param tar Shader type (ignored)
	 */
	void RelinkShader(ShaderType tar = NONE_SHADER) override;
	
	/**
	 * @brief Generates shader code (unused in ComputeShader).
	 */
	void GenerateShader(ShaderType tar = NONE_SHADER) override {};

	/**
	 * @brief Returns pointer to compute shader unit.
	 * @param tar Shader type (ignored)
	 * @return Pointer to comp_shader
	 */
	ShaderUnit* GetShaderUnit(ShaderType tar = NONE_SHADER) override;

public:

	/**
	 * @brief Dispatches compute shader scaled to screen resolution.
	 * 
	 * Automatically calculates workgroup count based on screen size and batch size.
	 * Handles edge cases with optional padding.
	 * 
	 * @param _scr_size Screen size (width, height)
	 * @param _batch Workgroup size (e.g., 16 for 16x16 workgroups)
	 * @param _edge_fix If true, adds padding for non-divisible resolutions
	 * 
	 * @note Common batch sizes: 8, 16, 32 (must match GLSL local_size_x/y)
	 */
	void RunComputeShaderSCR(const glm::vec2& _scr_size, GLuint _batch, bool _edge_fix = true);
	
	/**
	 * @brief Dispatches compute shader with explicit size.
	 * @param _size Dispatch size (workgroups will be calculated)
	 * @note Assumes 16x16 workgroup size
	 */
	void RunComputeShader(const glm::vec2& _size);
	
	/**
	 * @brief Dispatches compute shader with explicit workgroup counts.
	 * 
	 * Direct dispatch with specified workgroup counts in each dimension.
	 * 
	 * @param workgroup_count_x Number of workgroups in X dimension
	 * @param workgroup_count_y Number of workgroups in Y dimension
	 * @param workgroup_count_z Number of workgroups in Z dimension (default 1)
	 * 
	 * @note Total threads = workgroup_count * local_size (from GLSL)
	 */
	void RunComputeShader(GLuint workgroup_count_x = 1, GLuint workgroup_count_y = 1, GLuint workgroup_count_z = 1) const;

public:

	/**
	 * @brief Returns OpenGL shader ID for compute stage.
	 * @param type Shader type (ignored)
	 * @return Compute shader object ID
	 */
	inline GLuint GetShaderID(ShaderType type) const override;
	
	/**
	 * @brief Returns list of shader stages.
	 * @return Vector containing only COMPUTE_SHADER
	 */
	std::vector<ShaderType> GetAllShaderTypes() const override { return { COMPUTE_SHADER }; };
	
	/**
	 * @brief Prints debug information about compute shader.
	 */
	void LocalDebug() const override;

public:

	/**
	 * @brief Imports or retrieves a cached compute shader.
	 * @param _name Compute shader file name
	 * @return Reference to cached ComputeShader instance
	 * @note Returns cached instance from AssetManager if already loaded
	 */
	static ComputeShader& ImportShader(std::string _name);
	
	/**
	 * @brief Imports or retrieves a cached compute shader with default uniforms.
	 * @tparam Tuples Variadic template for uniform tuples
	 * @param _name Compute shader file name
	 * @param args Uniform tuples created with Uni() macro
	 * @return Reference to cached ComputeShader instance
	 */
	template<class... Tuples>
	static ComputeShader& ImportShader(std::string _name, const Tuples&... args);
	
	/**
	 * @brief Imports compute shader and returns shared_ptr.
	 * @param _name Compute shader file name
	 * @return Shared pointer to ComputeShader
	 */
	static std::shared_ptr<ComputeShader> ImportShaderSrc(std::string _name);
	
	/**
	 * @brief Imports compute shader with default uniforms and returns shared_ptr.
	 * @tparam Tuples Variadic template for uniform tuples
	 * @param _name Compute shader file name
	 * @param args Uniform tuples
	 * @return Shared pointer to ComputeShader
	 */
	template<class... Tuples>
	static std::shared_ptr<ComputeShader> ImportShaderSrc(std::string _name, const Tuples&... args);
	
	/**
	 * @brief Stores default uniform configs for a shader.
	 * @tparam Tuples Variadic template for uniform tuples
	 * @param _name Shader name
	 * @param args Uniform tuples
	 */
	template<class... Tuples>
	static void ImportShaderConfigs(std::string _name, const Tuples&... args);

	/**
	 * @brief Returns shader name for SSR based on config.
	 * @param config Render configuration
	 * @return Shader name string (e.g., "ssr_comp", "ssr_hq_comp")
	 */
	static std::string GetSSRShaderName(RenderConfigs* config);
	
	/**
	 * @brief Returns shader name for ambient occlusion based on config.
	 * @param config Render configuration
	 * @return Shader name string (e.g., "ssao_comp", "gtao_comp")
	 */
	static std::string GetAOShaderName(RenderConfigs* config);
	
	/**
	 * @brief Returns shader name for anti-aliasing based on config.
	 * @param config Render configuration
	 * @return Shader name string (e.g., "fxaa_comp", "smaa_comp")
	 */
	static std::string GetAAShaderName(RenderConfigs* config);
	
	/**
	 * @brief Returns shader name for shadow rendering.
	 * @param _type Shadow type (hard, soft, contact, etc.)
	 * @param _light_type Light type (point, sun, spot, area)
	 * @return Shader name string
	 */
	static std::string GetShadowShaderName(char _type, char _light_type);

	/**
	 * @brief Initializes compute shader library with default shaders.
	 * @param config Render configuration for shader variant selection
	 * @note Loads all post-processing and shadow compute shaders
	 */
	static void InitComputeLib(RenderConfigs* config);
	
	/**
	 * @brief Resets compute shader library and clears cache.
	 * @note Releases all cached compute shaders
	 */
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
	auto cached = AssetManager::Get<ComputeShader>(_name);
	if (cached)
		return cached;

	auto shader = std::make_shared<ComputeShader>(_name, args...);
	AssetManager::Register<ComputeShader>(_name, shader);
	ImportShaderConfigs(_name, args...);
	return shader;
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
