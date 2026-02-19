/**
 * @file Parameters.h
 * @brief Generic parameter system for exposing tweakable properties to UI.
 * 
 * Parameters provides a type-safe, runtime-configurable parameter system for
 * exposing shader uniforms, effect properties, and other tweakable values to
 * the editor UI. It supports common types (float, int, vec3, etc.) and stores
 * them in a type-erased std::variant for flexible access.
 * 
 * Architecture:
 * - Used by PostProcessing effects, shaders, and other configurable systems
 * - UI layer reads Parameters to generate sliders, color pickers, etc.
 * - Parameter changes propagate via dirty flags (is_para_changed)
 * - Values are validated against data_range constraints
 * 
 * @note Parameters are UI-facing and not directly GPU-uploadable (convert to uniforms).
 * @note Type safety enforced at compile-time via template Get<T>() accessor.
 */

#pragma once

#include "glm/glm.hpp"

#include <string>
#include <unordered_map>
#include <variant>

/**
 * @brief Enumeration of supported parameter types.
 */
enum ParaType
{
	NONE_PARA = -1, ///< Uninitialized or invalid parameter

	FLOAT_PARA,   ///< Single precision float
	INT_PARA,     ///< Signed integer
	BOOL_PARA,    ///< Boolean (true/false)
	STRING_PARA,  ///< UTF-8 string
	VEC2_PARA,    ///< 2D vector (glm::vec2)
	VEC3_PARA,    ///< 3D vector (glm::vec3)
	VEC4_PARA,    ///< 4D vector (glm::vec4)

	MAT3_PARA,    ///< 3x3 matrix (glm::mat3)
	MAT4_PARA,    ///< 4x4 matrix (glm::mat4)
	TEXTURE_PARA, ///< Texture reference (path or ID)
	CUSTOM_PARA   ///< User-defined custom type
};

/**
 * @brief Generic parameter container with type-safe runtime access.
 * 
 * Parameters encapsulates a named, typed value with optional range constraints.
 * It uses std::variant for type erasure, allowing heterogeneous parameter
 * collections to be stored in containers. The Get<T>() template accessor
 * provides type-safe retrieval with automatic type checking.
 * 
 * Dirty Flag:
 * - is_para_changed tracks whether value has been modified since last read
 * - Consumer code should reset flag after processing change
 * 
 * Range Constraints:
 * - data_range defines min/max bounds for numeric parameters
 * - UI layer uses this to configure slider ranges
 * - Enforcement is UI-side (not validated in Parameters itself)
 * 
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 * @note Type conversion: Get<T>() will default-initialize if type mismatch occurs.
 */
class Parameters
{
public:

	/**
	 * @brief Constructs a default parameter with no type.
	 * @note Type is NONE_PARA until assigned.
	 */
	Parameters();
	
	/**
	 * @brief Constructs a parameter with specified type and name.
	 * @param _type Parameter type (FLOAT_PARA, VEC3_PARA, etc.)
	 * @param _name Human-readable parameter name for UI display
	 */
	Parameters(ParaType _type, const std::string& _name = "unknown param");
	
	/**
	 * @brief Destroys the parameter.
	 */
	~Parameters();

public:

	/**
	 * @brief Internal storage for parameter value and range.
	 */
	struct ParaData
	{
		ParaData() {}
		~ParaData() {}
		std::variant<float, int, std::string, bool, glm::vec2, glm::vec3, glm::vec4> _data; ///< Type-erased value storage
		glm::vec2 data_range{ -10.0f, 10.0f }; ///< Min/max range for numeric types (used by UI sliders)
	};

	bool is_para_changed;      ///< Dirty flag indicating value changed since last read
	std::string para_name;     ///< Human-readable parameter name (displayed in UI)
	ParaType para_type;        ///< Runtime type discriminator
	ParaData para_data;        ///< Actual value storage

	/**
	 * @brief Type-safe accessor for parameter value.
	 * @tparam T Expected type (must match variant alternatives)
	 * @return Reference to stored value (default-initialized if type mismatch)
	 * @note If T doesn't match stored type, value is replaced with T(0).
	 */
	template<typename T>
	T& Get();

public:

	/**
	 * @brief Generates a new parameter item of specified type.
	 * @param type Type for the new parameter
	 * @return New Parameters instance with specified type
	 * @note Used for creating parameter collections programmatically.
	 */
	Parameters GenParaItem(ParaType type);
	
	/**
	 * @brief Returns pointer to internal parameter data.
	 * @return Pointer to ParaData for direct access
	 * @note Use with caution; prefer Get<T>() for type safety.
	 */
	ParaData* GetParaPtr() { return &para_data; }
	
	/**
	 * @brief Renames the parameter.
	 * @param name New display name for UI
	 */
	void Rename(const std::string& name) { para_name = name; }
};

template<typename T>
T& Parameters::Get()
{
	if (!std::holds_alternative<T>(para_data._data)) {
		para_data._data = T(0);
		para_type = (ParaType)para_data._data.index();
	}

	return std::get<T>(para_data._data);
}