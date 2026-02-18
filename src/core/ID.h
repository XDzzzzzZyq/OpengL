/**
 * @file ID.h
 * @brief Unique identifier system for all scene objects.
 * 
 * Provides base UID class for unique ID generation and ObjectID for scene graph objects.
 * Every object in the scene hierarchy inherits from ObjectID to enable identity tracking,
 * type discrimination, and polymorphic component access.
 * 
 * Architecture:
 * - UID provides globally unique integer IDs
 * - ObjectID extends UID with scene-specific metadata (name, type, visibility)
 * - All scene objects (Camera, Light, Mesh, etc.) inherit from ObjectID
 * - ID-based lookups enable efficient object management and serialization
 */

#pragma once

#include <string>

/**
 * @brief Base class providing globally unique integer IDs.
 * 
 * UID generates sequential unique IDs for all instances. Each UID tracks
 * a monotonically increasing ID counter to ensure no collisions.
 * 
 * @note Thread-safety: Not thread-safe. IDs should be allocated on main thread.
 * @note Lifetime: IDs are never reused, even after object destruction.
 */
class UID {
private:
	int id;          ///< Unique identifier for this instance
	static int count; ///< Global counter for ID generation

public:
	/**
	 * @brief Constructs a UID and assigns a unique ID.
	 * @note ID is assigned from global counter and incremented atomically.
	 */
	UID();
	
	/**
	 * @brief Virtual destructor for polymorphic use.
	 */
	virtual ~UID() = default;

	/**
	 * @brief Returns the unique ID of this object.
	 * @return Unique integer identifier
	 */
	inline int GetObjectID() const { return id; };
	
	/**
	 * @brief Returns total number of UIDs allocated.
	 * @return Total allocation count (includes destroyed objects)
	 */
	static int GetTotalAllocated() { return count; };
};

/**
 * @brief Base class for all scene graph objects with type and component access.
 * 
 * ObjectID extends UID with scene-specific functionality:
 * - Object name for display in editor UI
 * - Type enumeration for runtime type discrimination
 * - Visibility flags for viewport and rendering
 * - Polymorphic accessors for optional components (Transform, Shader, Material)
 * 
 * All scene objects inherit from ObjectID to participate in the scene graph.
 * Type-specific data is stored in derived classes (Camera, Light, Mesh, etc.).
 * 
 * @note Ownership: ObjectID instances are typically owned by shared_ptr in scene containers.
 * @note Component pattern: Uses void* accessors for optional components to avoid circular dependencies.
 */
class ObjectID : public UID
{
public:
	/**
	 * @brief Enumeration of scene object types.
	 * 
	 * Used for runtime type identification without RTTI. Enables type-specific
	 * rendering paths and UI display.
	 */
	enum GOType
	{
		NONE_GO = -1,   ///< Invalid or uninitialized object
		GO_CAM,         ///< Camera object
		GO_MESH,        ///< Mesh geometry
		GO_LIGHT,       ///< Light source (point, sun, spot, area)
		GO_POLYLIGHT,   ///< Polygonal area light
		GO_ENVIR,       ///< Environment map for IBL
		GO_SPRITE,      ///< 2D sprite
		GO_DL,          ///< Debug line primitive
		GO_DP,          ///< Debug point primitive
		GO_DM,          ///< Debug mesh
		GO_SDFFIELD     ///< SDF volume for soft shadows
	};

public:
	std::string o_name;          ///< Display name in editor UI
	GOType o_type = NONE_GO;     ///< Runtime type identifier

	mutable bool is_viewport = true; ///< Visible in viewport (editor-only)
	mutable bool is_rendered = true; ///< Included in rendering pipeline
	
	/**
	 * @brief Sets visibility flags for viewport and rendering.
	 * @param v Viewport visibility (editor display)
	 * @param r Render visibility (included in render passes)
	 */
	void SetVisible(bool v, bool r) {
		is_viewport = v;
		is_rendered = r;
	}

	/**
	 * @brief Returns pointer to object's shader, if applicable.
	 * @return Void pointer cast to Shader*, or nullptr if no shader
	 * @note Override in derived classes that own shaders (Mesh, Material, etc.)
	 */
	virtual void* GetShader()		{ return nullptr; }
	
	/**
	 * @brief Returns pointer to object's transform, if applicable.
	 * @return Void pointer cast to Transform*, or nullptr if no transform
	 * @note Override in derived classes with Transform component (Camera, Light, Mesh)
	 */
	virtual void* GetTransform()	{ return nullptr; }
	
	/**
	 * @brief Returns pointer to object's material, if applicable.
	 * @return Void pointer cast to Material*, or nullptr if no material
	 * @note Override in derived classes with Material component (Mesh)
	 */
	virtual void* GetMaterial()		{ return nullptr; }

	/**
	 * @brief Constructs an ObjectID with default values.
	 */
	ObjectID();
	
	/**
	 * @brief Destroys the ObjectID.
	 */
	~ObjectID();
};
