/**
 * @file Shaders.h
 * @brief Base shader abstraction for GPU shader compilation, linking, and uniform management.
 * 
 * Provides the foundational Shaders class that manages OpenGL shader programs, including
 * compilation of shader units (vertex, fragment, compute, geometry), linking into programs,
 * and runtime uniform setting. This is the base class for specialized shader types
 * (RenderShader, ComputeShader, etc.).
 * 
 * GPU Resource Ownership:
 * - Shaders owns the OpenGL program object (program_id)
 * - Shader units (vertex/fragment/etc.) are owned by derived classes
 * - Resources are deleted in destructor via glDeleteProgram
 * 
 * Architecture:
 * - Pure virtual methods enforce shader type-specific behavior
 * - Derived classes implement shader parsing, compilation, and linking
 * - Uniform cache minimizes redundant glGetUniformLocation calls
 */

#pragma once

#include "ShaderLib.h"

#include <optional>
#include <functional>

/**
 * @brief Types of array data supported for uniform arrays.
 */
enum ArrayType
{
	NULL_ARRAY,   ///< Null/uninitialized array
	VEC1_ARRAY,   ///< Array of floats
	VEC2_ARRAY,   ///< Array of vec2
	VEC3_ARRAY,   ///< Array of vec3
	VEC4_ARRAY,   ///< Array of vec4
	MAT4_ARRAY    ///< Array of mat4
};

/**
 * @brief Shader stage types supported by the renderer.
 */
enum ShaderType
{
	NONE_SHADER = -1,    ///< Invalid/unspecified shader type
	VERTEX_SHADER,       ///< Vertex shader stage
	FRAGMENT_SHADER,     ///< Fragment (pixel) shader stage
	COMPUTE_SHADER,      ///< Compute shader stage (GPGPU)
	GEOMETRY_SHADER      ///< Geometry shader stage
};

/**
 * @brief Helper macro for creating uniform tuples.
 * 
 * Usage: Uni("uniform_name", value)
 */
#define Uni std::make_tuple

/**
 * @brief Base class for OpenGL shader programs.
 * 
 * Shaders provides the core functionality for shader compilation, linking, and uniform management.
 * It is abstract and must be subclassed by specific shader types (RenderShader, ComputeShader).
 * 
 * Lifecycle:
 * 1. Construct shader (load/parse code)
 * 2. Compile shader units (vertex, fragment, etc.)
 * 3. Link shader program
 * 4. Use shader and set uniforms
 * 5. Destroy shader (releases GPU resources)
 * 
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 * @note Resource ownership: Owns program_id, releases via glDeleteProgram in destructor.
 */
class Shaders {

public:

	static std::string const shader_type[4]; ///< Shader type name strings

	/**
	 * @brief Pair of vertex and fragment shader names for loading.
	 */
	struct ShaderPair {
		std::string verShader;  ///< Vertex shader file name
		std::string fragShader; ///< Fragment shader file name
	};

	/**
	 * @brief Individual shader compilation unit (vertex, fragment, compute, etc.).
	 * 
	 * Represents a single shader stage with its source code, compiled shader ID,
	 * and optional GLSL structure metadata. ShaderUnits are compiled into OpenGL
	 * shader objects (vertex, fragment, etc.) and then linked into a program.
	 * 
	 * GPU Resource Ownership:
	 * - Owns OpenGL shader object (sh_ID)
	 * - Released via glDeleteShader in destructor or _del()
	 * - Move semantics transfer ownership
	 */
	class ShaderUnit
	{
	private:
		void _del(); ///< Deletes the shader object via glDeleteShader
		void _copyInfo(const ShaderUnit& unit); ///< Copies metadata from another unit
		void _resetID(GLuint ID); ///< Resets shader ID, deleting old shader if needed
	public:
		ShaderType sh_type{ NONE_SHADER };        ///< Shader stage type (vertex, fragment, etc.)
		std::string sh_name{ "" };                ///< Shader name (usually file name)
		GLuint sh_ID{ 0 };                        ///< OpenGL shader object ID
		std::string sh_code{ "" };                ///< GLSL source code
		std::optional<ShaderStruct> sh_struct{};  ///< Parsed GLSL structure (uniforms, attributes)

		/**
		 * @brief Default constructor.
		 */
		ShaderUnit() {}
		
		/**
		 * @brief Constructs a shader unit with type and name.
		 * @param type Shader stage type
		 * @param name Shader name/file name
		 */
		ShaderUnit(ShaderType type, std::string name);
		
		/**
		 * @brief Copy constructor (deep copy of shader data).
		 * @param unit Source shader unit to copy
		 */
		ShaderUnit(const ShaderUnit& unit);
		
		/**
		 * @brief Move constructor (transfers ownership).
		 * @param unit Source shader unit to move from
		 */
		ShaderUnit(ShaderUnit&& unit) noexcept;
		
		/**
		 * @brief Destructor (releases OpenGL shader object).
		 */
		~ShaderUnit();

		/**
		 * @brief Copy assignment operator.
		 * @param unit Source shader unit to copy
		 * @return Reference to this shader unit
		 */
		ShaderUnit& operator=(const ShaderUnit& unit);
		
		/**
		 * @brief Move assignment operator (transfers ownership).
		 * @param unit Source shader unit to move from
		 * @return Reference to this shader unit
		 */
		ShaderUnit& operator=(ShaderUnit&& unit) noexcept;

		/**
		 * @brief Hash function for using ShaderUnit in unordered containers.
		 */
		struct hash_fn
		{
			std::size_t operator()(const ShaderUnit& inp) const;
		};
	};

	/**
	 * @brief Array uniform data container.
	 * 
	 * Used for passing arrays of data to shader uniforms (e.g., light arrays,
	 * bone matrices, etc.).
	 */
	struct ArrayUni 
	{
		GLuint size;      ///< Number of elements in the array
		float* data;      ///< Pointer to array data
		ArrayType type;   ///< Type of array (vec1, vec2, vec3, vec4, mat4)
	};

public:

	/**
	 * @brief Default constructor.
	 */
	Shaders() {}
	
	/**
	 * @brief Copy constructor.
	 * @param shader Source shader to copy
	 */
	Shaders(const Shaders& shader);
	
	/**
	 * @brief Move constructor (transfers ownership of GPU resources).
	 * @param shader Source shader to move from
	 */
	Shaders(Shaders&& shader) noexcept;
	
	/**
	 * @brief Destructor (releases OpenGL program object).
	 */
	~Shaders();

	/**
	 * @brief Copy assignment operator.
	 * @param shader Source shader to copy
	 * @return Reference to this shader
	 */
	Shaders& operator=(const Shaders& shader);
	
	/**
	 * @brief Move assignment operator (transfers ownership).
	 * @param shader Source shader to move from
	 * @return Reference to this shader
	 */
	Shaders& operator=(Shaders&& shader) noexcept;

protected:

	/**
	 * @brief Deletes the OpenGL program object.
	 */
	void _del();
	
	/**
	 * @brief Resets program ID, deleting old program if needed.
	 * @param _ID New program ID to assign
	 */
	void _resetProgramID(GLuint _ID) { if (program_id > 0 && program_id != _ID)_del(); program_id = _ID; }

public:

	/**
	 * @brief Shader metadata tuple: (name, filename, GL_enum).
	 */
	using ShaderConstInfo = std::tuple<std::string, std::string, GLuint>;

	/**
	 * @brief Parses shader type to metadata tuple.
	 * @param _type Shader type enum
	 * @return Tuple of (name, filename extension, OpenGL enum)
	 */
	static ShaderConstInfo ParseShaderType(ShaderType _type);
	
	/**
	 * @brief Infers shader type from file extension.
	 * @param path Shader file path
	 * @return ShaderType inferred from extension (.vert, .frag, .comp, etc.)
	 */
	static ShaderType ParseFileEXT(std::string path);

	/**
	 * @brief Compiles GLSL source code into an OpenGL shader object.
	 * @param _type Shader stage type (vertex, fragment, etc.)
	 * @param source GLSL source code string
	 * @return OpenGL shader object ID, or 0 on compilation failure
	 */
	static GLuint CompileShaderCode(ShaderType _type, const std::string& source);
	
	/**
	 * @brief Reads shader source from file.
	 * @param _type Shader stage type
	 * @param name Shader file name (without path or extension)
	 * @return GLSL source code as string
	 */
	static std::string ReadShaderFile(ShaderType _type, const std::string& name);

	static std::string folder_root;          ///< Root directory for shader files
	static std::vector<std::string> file_type; ///< File extensions for shader types

public:

	GLuint program_id{ 0 }; ///< OpenGL shader program ID (owned by this class)

protected:

	mutable std::unordered_map<std::string, GLuint> _uniforms_cache; ///< Cache of uniform locations (avoids redundant glGetUniformLocation)
	std::unordered_map<std::string, int> _LINK_LOC;                  ///< Map of linked uniform locations

	/**
	 * @brief Checks if a uniform name has already been linked.
	 * @param _name Uniform name to check
	 * @return True if uniform already linked
	 */
	bool _is_link_repeat(const std::string _name) { for (auto& i : _LINK_LOC) if (_name == i.first)return true; return false; }

public:

	bool is_shader_changed{ true }; ///< Flag indicating shader code has changed and needs recompilation
	
	/**
	 * @brief Returns the OpenGL program ID.
	 * @return Program ID
	 */
	inline GLuint GetProgramID() const { return program_id; }
	
	/**
	 * @brief Returns the OpenGL shader object ID for a specific stage.
	 * @param type Shader stage type
	 * @return Shader object ID
	 */
	virtual GLuint GetShaderID(ShaderType type) const = 0;
	
	/**
	 * @brief Returns list of all shader stages in this program.
	 * @return Vector of ShaderType enums
	 */
	virtual std::vector<ShaderType> GetAllShaderTypes() const = 0;

	/**
	 * @brief Resets the shader ID for a specific stage.
	 * @param type Shader stage type
	 * @param id New OpenGL shader object ID
	 */
	virtual void ResetID(ShaderType type, GLuint id) = 0;
	
	/**
	 * @brief Relinks the shader program after shader unit changes.
	 * @param tar Target shader stage to relink, or NONE_SHADER to relink all
	 */
	virtual void RelinkShader(ShaderType tar = NONE_SHADER) = 0;

	/**
	 * @brief Parses GLSL source code for a shader stage.
	 * @param _code GLSL source code
	 * @param _type Shader stage type
	 */
	virtual void ParseShaderCode(const std::string& _code, ShaderType _type) = 0;
	
	/**
	 * @brief Generates shader code (for procedural shaders).
	 * @param tar Target shader stage, or NONE_SHADER for all
	 */
	virtual void GenerateShader(ShaderType tar = NONE_SHADER) = 0;
	
	/**
	 * @brief Returns pointer to shader unit for a specific stage.
	 * @param tar Target shader stage, or NONE_SHADER for default
	 * @return Pointer to ShaderUnit
	 */
	virtual ShaderUnit* GetShaderUnit(ShaderType tar = NONE_SHADER) = 0;
	
	/**
	 * @brief Compiles a shader stage.
	 * @param tar Target shader stage to compile
	 * @return OpenGL shader object ID, or 0 on failure
	 */
	virtual GLuint CompileShader(ShaderType tar) = 0;

public:

	std::function<void(void)> InitShader{}; ///< Optional callback for custom shader initialization
	
	/**
	 * @brief Clears the uniform location cache.
	 * 
	 * Forces uniform locations to be re-queried on next SetValue call.
	 * Should be called after shader relinking.
	 */
	void ResetCache() { _uniforms_cache = {}; }

	/**
	 * @brief Binds this shader program for rendering.
	 * 
	 * Calls glUseProgram with this program's ID. Must be called before
	 * setting uniforms or issuing draw calls.
	 */
	void UseShader() const;
	
	/**
	 * @brief Unbinds this shader program.
	 * 
	 * Calls glUseProgram(0). Should be called after rendering is complete.
	 */
	void UnuseShader() const;

public:

	/**
	 * @brief Gets uniform location ID for a uniform variable.
	 * @param name Uniform variable name
	 * @return OpenGL uniform location ID
	 * @note Uses cache to avoid redundant glGetUniformLocation calls
	 */
	inline GLuint getVarID(const char* name) const;

	/** @name Uniform Setters
	 * Methods for setting shader uniforms with type safety.
	 * Each SetValue overload corresponds to a GLSL uniform type.
	 * @note Shader must be bound via UseShader() before calling these.
	 */
	///@{
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
	void SetValue(const std::string& name, GLsizei count, const float* va0, ArrayType TYPE);
	void SetValue(const std::string& name, Shaders::ArrayUni arr);
	void SetValue(const std::string& name, GLsizei count, const int* va0, ArrayType TYPE);
	void SetValue(const std::string& name, GLsizei count, const GLuint* va0, ArrayType TYPE);
	void SetValue(const std::string& name, GLsizei count, const glm::mat4* va0);
	///@}

public:

	/**
	 * @brief Prints debug information about the shader program.
	 * 
	 * Logs shader units, uniforms, attributes, and compilation status.
	 * Used for debugging shader issues.
	 */
	virtual void LocalDebug() const = 0;

};
