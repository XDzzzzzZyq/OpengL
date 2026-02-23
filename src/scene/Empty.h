/**
 * @file Empty.h
 * @brief Empty transform node for scene hierarchy organization.
 * 
 * Empty is a lightweight scene object with no renderable content, used as a
 * transform node for organizing hierarchies, parenting groups of objects, or
 * storing metadata. It inherits from ObjectID (scene identity) and Transform3D
 * (spatial placement).
 * 
 * Architecture:
 * - Owned by scene graph (shared_ptr in Scene containers)
 * - Never rendered directly (has no geometry or visual representation)
 * - Used as parent for grouping objects with shared transforms
 * - Can act as anchor point for post-processing effects or field volumes
 * 
 * @note Empty objects have no GPU resource overhead (no buffers or textures).
 * @note Useful for scene organization without performance impact.
 */

#pragma once
#include "Transform.h"
#include "ID.h"

/**
 * @brief Enumeration of Empty node semantic types.
 */
enum EmptyType
{
	BLANK_EMPTY,      ///< Generic empty node (pure transform)
	POST_PROC_EMPTY,  ///< Anchor for post-processing effect
	FIELD_EMPTY       ///< Volume field definition (e.g., SDF bounds)
};

/**
 * @brief Empty transform node for scene hierarchy organization.
 * 
 * Empty provides a transform node with no visual representation. It is used
 * for organizing scene hierarchies, grouping objects, and marking special
 * locations (e.g., effect anchors, volume bounds). Empty nodes participate
 * in parent-child transform propagation but are never rendered.
 * 
 * Use Cases:
 * - Grouping multiple objects under a common parent transform
 * - Marking anchor points for procedural effects
 * - Defining volume bounds for field computations (SDF, fog, etc.)
 * - Placeholder for future object instantiation
 * 
 * Rendering Pipeline Integration:
 * - Skipped during geometry pass (no draw calls)
 * - Transform propagated to children normally
 * - May trigger special behavior based on empty_type (e.g., POST_PROC_EMPTY)
 * 
 * @note Inheritance: ObjectID for scene identity, Transform3D for spatial transform.
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 * @note Minimal overhead: No GPU resources allocated.
 */
class Empty : public ObjectID, public Transform3D
{
private:
	EmptyType empty_type; ///< Semantic type of this Empty node
public:
	/**
	 * @brief Constructs an Empty node with specified type.
	 * @param type Semantic type (BLANK_EMPTY, POST_PROC_EMPTY, FIELD_EMPTY)
	 */
	Empty(EmptyType type);
	
	/**
	 * @brief Destroys the Empty node.
	 */
	~Empty();
};

