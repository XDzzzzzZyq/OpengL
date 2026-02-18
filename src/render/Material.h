/**
 * @file Material.h
 * @brief Material system for PBR rendering with texture and parameter management.
 * 
 * Provides the Material class for representing physically-based rendering (PBR) materials
 * with support for textures, constants, and procedural shading. Materials are used by
 * Mesh objects to define surface appearance (albedo, metallic, roughness, etc.).
 * 
 * GPU Resource Ownership:
 * - Material owns TextureRes shared pointers (reference-counted textures)
 * - Does not own GPU texture objects directly (managed by TextureLib)
 * - Materials can be shared across multiple meshes
 * 
 * Architecture:
 * - Each parameter can be a float, color (vec3), or texture
 * - Texture binding happens in BindMatTexture()
 * - Change tracking via is_mat_changed flags for efficient updates
 */

#pragma once

#include "Parameters.h"
#include "Texture.h"

#include <array>

/**
 * @brief Material descriptor for PBR rendering.
 * 
 * Material encapsulates all surface appearance parameters for physically-based rendering.
 * Supports hybrid mode where each parameter can be a constant, color, or texture map.
 * 
 * Material Parameters:
 * - MAT_ALBEDO: Base color / diffuse albedo
 * - MAT_METAL: Metallic factor (0 = dielectric, 1 = conductor)
 * - MAT_ROUGH: Roughness (0 = smooth/mirror, 1 = rough/matte)
 * - MAT_SPEC: Specular reflectance (non-metallic materials)
 * - MAT_EMIS_COL: Emissive color
 * - MAT_EMIS_STR: Emissive strength multiplier
 * - MAT_ALPHA: Opacity/transparency
 * - MAT_NORMAL: Normal map (tangent space)
 * - MAT_BUMP: Bump/height map
 * 
 * Usage:
 * @code
 * auto mat = Material::LoadMaterial("materials/gold.mat");
 * mat->SetMatParam(Material::MAT_ALBEDO, glm::vec3(1.0f, 0.766f, 0.336f));
 * mat->SetMatParam(Material::MAT_METAL, 1.0f);
 * mat->SetMatParam(Material::MAT_ROUGH, 0.3f);
 * mat->BindMatTexture();  // Bind before rendering
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. Material state should not be modified during rendering.
 */
class Material
{
public:
	
	/**
	 * @brief Material parameter types.
	 * 
	 * Each parameter can have different data types depending on the material setup.
	 * Constants provide uniform values, while textures provide per-pixel variation.
	 */
	enum MatParaType
	{
		MAT_NONE = -1,  ///< Invalid/uninitialized parameter
		MAT_ALBEDO,     ///< Base color (vec3 or texture)
		MAT_METAL,      ///< Metallic factor (float or texture)
		MAT_ROUGH,      ///< Roughness factor (float or texture)
		MAT_SPEC,       ///< Specular reflectance (vec3 or texture)
		MAT_EMIS_COL,   ///< Emissive color (vec3 or texture)
		MAT_EMIS_STR,   ///< Emissive strength (float multiplier)
		MAT_ALPHA,      ///< Opacity (float or texture)
		MAT_NORMAL,     ///< Normal map (texture, tangent space)
		MAT_BUMP,       ///< Bump/height map (texture)
		MAT_END         ///< Sentinel value (count of parameters)
	};

	/**
	 * @brief Data type discriminator for material parameters.
	 * 
	 * Specifies how a parameter is stored and should be interpreted by shaders.
	 */
	enum MatDataType
	{
		MPARA_FLT,   ///< Float constant (single value)
		MPARA_COL,   ///< Color constant (vec3)
		MPARA_TEX,   ///< Texture map (sampler2D)
		MPARA_CODE   ///< Procedural/shader code (reserved for future use)
	};

	using MatParamData = std::tuple<MatDataType, float, glm::vec3, TextureLib::TextureRes>; ///< Parameter data: (type, float, vec3, texture)
	using MaterialRes = std::shared_ptr<Material>; ///< Shared pointer to Material (for resource sharing)
	
	static std::vector<std::string> mat_uniform_name; ///< Uniform names for each MatParaType (indexed array)

public:

	std::string mat_name{ "Default" }; ///< Material name (for debugging and serialization)
	std::unordered_map<MatParaType, MatParamData> mat_params; ///< Map of material parameters to their data
	
	/**
	 * @brief Initializes all material parameters to default values.
	 * 
	 * Sets up default PBR parameters:
	 * - Albedo: white (1, 1, 1)
	 * - Metallic: 0.0
	 * - Roughness: 0.5
	 * - Specular: gray (0.04, 0.04, 0.04)
	 * - Emissive: black, strength 0
	 * - Alpha: 1.0 (opaque)
	 */
	void InitParamData();

public:

	/**
	 * @brief Default constructor (creates default PBR material).
	 */
	Material();
	
	/**
	 * @brief Destructor.
	 */
	~Material();

	/**
	 * @brief Loads a material from a configuration file.
	 * @param _path Path to material file (JSON or custom format)
	 * @return Shared pointer to loaded Material, or default material on failure
	 * @note If path is empty, returns default material
	 */
	static MaterialRes LoadMaterial(std::string _path = "");
	
	/**
	 * @brief Parses material configuration from a string.
	 * @param _config Material configuration string (JSON or custom format)
	 * @note Updates mat_params based on parsed config
	 */
	void ParseConfig(const std::string& _config);

public:

	bool is_mat_changed{ true };        ///< Flag indicating material parameters have changed
	bool is_mat_struct_changed{ true }; ///< Flag indicating material structure (data types) has changed
	
	/**
	 * @brief Sets the data type for a material parameter.
	 * @param _tar Parameter type (MAT_ALBEDO, MAT_ROUGH, etc.)
	 * @param _type Data type (MPARA_FLT, MPARA_COL, MPARA_TEX)
	 * @note Marks material as changed for GPU update
	 */
	void SetMatParam(MatParaType _tar, MatDataType _type);
	
	/**
	 * @brief Sets a material parameter to a float value.
	 * @param _tar Parameter type
	 * @param _var Float value
	 * @note Automatically sets data type to MPARA_FLT
	 */
	void SetMatParam(MatParaType _tar, float _var);
	
	/**
	 * @brief Sets a material parameter to a color value.
	 * @param _tar Parameter type
	 * @param _col Color value (vec3, RGB in [0,1])
	 * @note Automatically sets data type to MPARA_COL
	 */
	void SetMatParam(MatParaType _tar, glm::vec3 _col);
	
	/**
	 * @brief Sets a material parameter to a texture.
	 * @param _tar Parameter type
	 * @param _tex Texture resource (shared pointer)
	 * @note Automatically sets data type to MPARA_TEX
	 */
	void SetMatParam(MatParaType _tar, TextureLib::TextureRes _tex);

	/**
	 * @brief Binds all material textures to their respective texture units.
	 * 
	 * Iterates through mat_params and binds all MPARA_TEX type parameters
	 * to OpenGL texture units. Should be called before rendering with this material.
	 * 
	 * Texture Unit Assignment:
	 * - Typically uses units 0-9 for material textures
	 * - Specific slot assignment depends on shader uniform locations
	 * 
	 * @note Must be called within an active OpenGL context
	 * @note Corresponding shader must have matching sampler2D uniforms
	 */
	void BindMatTexture() const;

};
