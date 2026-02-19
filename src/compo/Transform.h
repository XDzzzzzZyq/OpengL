/**
 * @file Transform.h
 * @brief Transform component system for 2D and 3D spatial hierarchies.
 * 
 * Provides base Transform interface and concrete Transform3D/Transform2D implementations
 * for scene graph spatial representation. Transforms support hierarchical parent-child
 * relationships with position/rotation/scale inheritance.
 * 
 * Architecture:
 * - Transform is abstract base with type-agnostic interface
 * - Transform3D for 3D objects (Camera, Light, Mesh) using glm::mat4
 * - Transform2D for 2D objects (Sprite, UI) using glm::mat3
 * - Dirty flag system minimizes matrix recomputation
 * - Parent-child relationships enable skeletal hierarchies
 * 
 * @note All scene objects with spatial placement inherit Transform3D or Transform2D.
 * @note Transforms are not thread-safe. Access from main thread only.
 */

#pragma once

#include "glm/glm.hpp"
#include <GL/glew.h>

// GLM documentation: https://glm.g-truc.net/0.9.9/api/a00356.html

/**
 * @brief Abstract base class for transform components.
 * 
 * Transform provides common interface for position/rotation/scale manipulation
 * and hierarchical relationships. Derived classes implement dimension-specific
 * matrix mathematics (mat3 for 2D, mat4 for 3D).
 * 
 * Dirty Flag System:
 * - is_TransF_changed: Position/rotation/scale modified
 * - is_invTransF_changed: Inverse transform needs recomputation
 * - is_Uniform_changed: Uniform (non-scaled) transform modified
 * - is_invUniform_changed: Inverse uniform transform needs recomputation
 * 
 * @note Abstract class - instantiate Transform3D or Transform2D instead.
 */
class Transform {

public:
	/**
	 * @brief Flags for enabling/disabling transform components.
	 */
	enum TransType {
		None,
		Position = (1 << 0), ///< Enable position component
		Rotation = (1 << 1), ///< Enable rotation component
		Scale	 = (1 << 2), ///< Enable scale component
	};

public:
	mutable bool is_TransF_changed = true;      ///< Transform matrix needs recomputation
	mutable bool is_invTransF_changed = true;   ///< Inverse transform needs recomputation
	mutable bool is_Uniform_changed = true;     ///< Uniform (non-scaled) transform needs recomputation
	mutable bool is_invUniform_changed = true;  ///< Inverse uniform transform needs recomputation

public:
	/**
	 * @brief Removes parent transform relationship.
	 * @param _keep_offset If true, maintains world position; if false, resets to parent's origin
	 */
	virtual void UnsetParent(bool _keep_offset = true) = 0;

public:
	bool use_position{ true }; ///< Enable position component in transform computation
	bool use_rotation{ true }; ///< Enable rotation component in transform computation
	bool use_scale{ true };    ///< Enable scale component in transform computation
	
	/**
	 * @brief Enables or disables specific transform components.
	 * @param _enable True to enable, false to disable
	 * @param _type Component type (Position, Rotation, Scale) as bit flags
	 * @note Useful for locking axes or constraining transforms.
	 */
	void UseTranformComp(bool _enable, TransType _type);

	/**
	 * @brief Recomputes transform matrix if dirty.
	 * @param _forced If true, recompute even if not dirty
	 * @return True if recomputation occurred
	 */
	virtual bool ApplyTransform(bool _forced = false) = 0;
	
	/**
	 * @brief Recomputes transform and all parent transforms.
	 * @return True if any recomputation occurred
	 */
	virtual bool ApplyAllTransform() = 0;
	
	/**
	 * @brief Computes inverse transform matrix.
	 * @return True if inverse was recomputed
	 */
	virtual bool GetInvTransform() const = 0;

public:
	/**
	 * @brief Debug utility for transform state inspection.
	 * @return Debug status code (implementation-defined)
	 */
	virtual int _debug() const = 0;
};

/**
 * @brief 3D transform component for scene objects (Camera, Light, Mesh).
 * 
 * Transform3D provides position, rotation (Euler angles), and scale in 3D space.
 * Supports hierarchical parent-child relationships for skeletal animation and
 * scene graph organization. Transform matrices are cached and recomputed lazily
 * using dirty flags.
 * 
 * Coordinate System:
 * - Right-handed coordinate system (OpenGL convention)
 * - Rotation order: pitch (X), yaw (Y), roll (Z)
 * - Direction vectors: up (Y+), right (X+), forward (Z-)
 * 
 * Transform Composition:
 * - Local transform = Translate * Rotate * Scale
 * - World transform = Parent * Local
 * 
 * @note Mutable members support const correctness while caching computed matrices.
 * @note Parent-child pointers are non-owning (scene graph owns objects).
 */
class Transform3D : public Transform
{
private:
	Transform3D* o_parent_trans{ nullptr }; ///< Non-owning pointer to parent transform
	Transform3D* o_child_trans{ nullptr };  ///< Non-owning pointer to first child transform

public:
	/**
	 * @brief Constructs a Transform3D with identity values.
	 */
	Transform3D();
	
	/**
	 * @brief Destroys the Transform3D.
	 */
	~Transform3D();

	/**
	 * @brief Returns pointer to this transform.
	 * @return Non-owning pointer to Transform3D
	 */
	Transform3D* GetTransformPtr() { return this; }
	
	/**
	 * @brief Returns pointer to parent transform.
	 * @return Non-owning pointer to parent Transform3D, or nullptr if no parent
	 */
	Transform3D* GetParentTransPtr() { return o_parent_trans; }
	
	/**
	 * @brief Returns pointer to first child transform.
	 * @return Non-owning pointer to child Transform3D, or nullptr if no children
	 */
	Transform3D* GetChildTransPtr() { return o_child_trans; }

public:
	mutable glm::mat4 o_Transform{ 1.0f };    ///< Cached world transform matrix
	mutable glm::mat4 o_InvTransform{ 1.0f }; ///< Cached inverse world transform matrix

	mutable glm::vec3 o_position{ 0.0f }; ///< World position

	mutable glm::vec3 o_scale{ 1.0f }; ///< Local scale (non-uniform allowed)

	mutable glm::vec3 o_rot{ 0.0f };              ///< Euler rotation angles (radians): pitch, yaw, roll
	mutable glm::vec3 o_dir_up{ 0.0f, 1.0f, 0.0f };    ///< Up direction vector (Y+)
	mutable glm::vec3 o_dir_right{ 1.0f, 0.0f, 0.0f }; ///< Right direction vector (X+)

public:
	/**
	 * @brief Sets world position.
	 * @param pos New position in world space
	 * @return True if position changed
	 */
	bool SetPos(const glm::vec3& pos);
	
	/**
	 * @brief Sets single axis of position.
	 * @param _1d New value for specified axis
	 * @param _dim Axis dimension (0=X, 1=Y, 2=Z)
	 * @return True if position changed
	 */
	bool SetPos1D(float _1d, GLuint _dim);
	
	/**
	 * @brief Sets local scale.
	 * @param scale New scale (per-axis)
	 * @return True if scale changed
	 */
	bool SetScale(const glm::vec3& scale);
	
	/**
	 * @brief Sets single axis of scale.
	 * @param _1d New value for specified axis
	 * @param _dim Axis dimension (0=X, 1=Y, 2=Z)
	 * @return True if scale changed
	 */
	bool SetScale1D(float _1d, GLuint _dim);
	
	/**
	 * @brief Sets rotation via Euler angles.
	 * @param rot New rotation (pitch, yaw, roll) in radians
	 * @return True if rotation changed
	 */
	bool SetRot(const glm::vec3& rot);
	
	/**
	 * @brief Sets single axis of rotation.
	 * @param _1d New rotation value in radians
	 * @param _dim Axis dimension (0=pitch, 1=yaw, 2=roll)
	 * @return True if rotation changed
	 */
	bool SetRot1D(float _1d, GLuint _dim);
	
	/**
	 * @brief Sets transform from 4x4 matrix.
	 * @param _trans Source transformation matrix
	 * @param pos Extract position component (default: true)
	 * @param rot Extract rotation component (default: true)
	 * @param scl Extract scale component (default: true)
	 * @return True if any component changed
	 */
	bool SetTrans(const glm::mat4& _trans, bool pos = true, bool rot = true, bool scl = true);

	/**
	 * @brief Applies transformation matrix to current transform.
	 * @param _trans Transformation to apply
	 * @note Multiplies current transform: Transform = Transform * _trans
	 */
	void Trans(const glm::mat4& _trans);
	
	/**
	 * @brief Translates position by offset.
	 * @param d_pos Delta position to add
	 */
	void Move(const glm::vec3& d_pos);
	
	/**
	 * @brief Rotates around arbitrary axis through anchor point.
	 * @param anch Anchor point (rotation center)
	 * @param axis Rotation axis (normalized)
	 * @param angle Rotation angle in radians
	 */
	void Spin(const glm::vec3& anch, const glm::vec3& axis, const float& angle);
	
	/**
	 * @brief Rotates around anchor with pitch/yaw.
	 * @param anch Anchor point (rotation center)
	 * @param angle Pitch (X) and yaw (Y) angles in radians
	 * @param global_up Use global up vector (Y+) vs. local up (default: true)
	 */
	void Spin(const glm::vec3& anch, const glm::vec2& angle, bool global_up = true);
	
	/**
	 * @brief Orients transform to look at target position.
	 * @param tar Target position to face
	 * @param up Up vector for orientation (default: Z+)
	 */
	void LookAt(const glm::vec3& tar, const glm::vec3& up = {0,0,1});

	/**
	 * @brief Sets parent transform for hierarchy.
	 * @param _p_trans Pointer to new parent transform
	 * @param _keep_offset If true, maintains world position; if false, becomes local to parent
	 */
	void SetParent(Transform3D* _p_trans, bool _keep_offset = true);
	
	/**
	 * @brief Removes parent transform relationship.
	 * @param _keep_offset If true, maintains world position; if false, resets to origin
	 */
	void UnsetParent(bool _keep_offset = true) override;

private:
	/**
	 * @brief Helper to set single component of vec3.
	 * @param _tar Target vector to modify
	 * @param _1d New value for dimension
	 * @param _dim Dimension index (0, 1, or 2)
	 * @return True if value changed
	 */
	bool _set1D(glm::vec3& _tar, float _1d, GLuint _dim);

	/**
	 * @brief Updates cached direction vectors from rotation.
	 * @note Recomputes o_dir_up and o_dir_right based on current o_rot.
	 */
	void _updateDirections();

public:
	/**
	 * @brief Recomputes transform matrix if dirty.
	 * @param _forced If true, recompute even if not dirty
	 * @return True if recomputation occurred
	 */
	bool ApplyTransform(bool _forced = false) override;
	
	/**
	 * @brief Recomputes transform and all parent transforms.
	 * @return True if any recomputation occurred
	 */
	bool ApplyAllTransform() override;
	
	/**
	 * @brief Computes inverse transform matrix.
	 * @return True if inverse was recomputed
	 */
	bool GetInvTransform() const override;

	/**
	 * @brief Debug utility for transform state inspection.
	 * @return Debug status code
	 */
	int _debug() const override;

};
/**
 * @brief 2D transform component for sprites and UI elements.
 * 
 * Transform2D provides position, rotation (single angle), and scale in 2D space.
 * Supports hierarchical parent-child relationships for UI layouts and sprite
 * hierarchies. Transform matrices are 3x3 (mat3) for 2D homogeneous coordinates.
 * 
 * Coordinate System:
 * - 2D plane: X (horizontal), Y (vertical)
 * - Rotation: counterclockwise angle in radians
 * - Transform order: Translate * Rotate * Scale
 * 
 * @note Mutable members support const correctness while caching computed matrices.
 * @note Parent-child pointers are non-owning (scene graph owns objects).
 */
class Transform2D : public Transform
{
private:
	Transform2D* o_parent_trans{ nullptr }; ///< Non-owning pointer to parent transform
	Transform2D* o_child_trans{ nullptr };  ///< Non-owning pointer to first child transform
	
public:
	/**
	 * @brief Constructs a Transform2D with identity values.
	 */
	Transform2D();
	
	/**
	 * @brief Destroys the Transform2D.
	 */
	~Transform2D();

	/**
	 * @brief Returns pointer to this transform.
	 * @return Non-owning pointer to Transform2D
	 */
	Transform2D* GetTransformPtr() { return this; }
	
	/**
	 * @brief Returns pointer to parent transform.
	 * @return Non-owning pointer to parent Transform2D, or nullptr if no parent
	 */
	Transform2D* GetParentTransPtr() { return o_parent_trans; }
	
	/**
	 * @brief Returns pointer to first child transform.
	 * @return Non-owning pointer to child Transform2D, or nullptr if no children
	 */
	Transform2D* GetChildTransPtr() { return o_child_trans; }
public:
	mutable glm::mat3 o_Transform{ 1.0f };    ///< Cached world transform matrix (3x3)
	mutable glm::mat3 o_InvTransform{ 1.0f }; ///< Cached inverse world transform matrix (3x3)

	mutable glm::vec2 o_position{ 0.0f }; ///< World position (X, Y)

	mutable glm::vec2 o_scale{ 1.0f }; ///< Local scale (non-uniform allowed)

	mutable float o_rot{ 0.0f }; ///< Rotation angle in radians (counterclockwise)

public:
	/**
	 * @brief Sets world position.
	 * @param pos New position in 2D space
	 * @return True if position changed
	 */
	bool SetPos(const glm::vec2& pos);
	
	/**
	 * @brief Sets local scale.
	 * @param scale New scale (per-axis)
	 * @return True if scale changed
	 */
	bool SetScale(const glm::vec2& scale);
	
	/**
	 * @brief Sets rotation angle.
	 * @param rot New rotation in radians (counterclockwise)
	 * @return True if rotation changed
	 */
	bool SetRot(float rot);

	/**
	 * @brief Applies transformation matrix to current transform.
	 * @param _trans Transformation to apply (3x3 matrix)
	 * @note Multiplies current transform: Transform = Transform * _trans
	 */
	void Trans(const glm::mat3& _trans);
	
	/**
	 * @brief Translates position by offset.
	 * @param d_pos Delta position to add
	 */
	void Move(const glm::vec2& d_pos);
	
	/**
	 * @brief Rotates by angle.
	 * @param angle Rotation angle in radians (counterclockwise)
	 */
	void Spin(float angle);
	
	/**
	 * @brief Scales by factor (uniform).
	 * @param scale Uniform scale multiplier
	 */
	void Zoom(float scale);
	
	/**
	 * @brief Scales by factor (non-uniform).
	 * @param scale Per-axis scale multiplier
	 */
	void Zoom(const glm::vec2& scale);
	
	/**
	 * @brief Orients transform to look at target position.
	 * @param tar Target position to face
	 * @note Sets rotation to face target from current position.
	 */
	void LookAt(const glm::vec2& tar); \

	/**
	 * @brief Sets parent transform for hierarchy.
	 * @param _p_trans Pointer to new parent transform
	 * @param _keep_offset If true, maintains world position; if false, becomes local to parent
	 */
	void SetParent(Transform2D* _p_trans, bool _keep_offset = true);
	
	/**
	 * @brief Removes parent transform relationship.
	 * @param _keep_offset If true, maintains world position; if false, resets to origin
	 */
	void UnsetParent(bool _keep_offset = true) override;
public:
	/**
	 * @brief Recomputes transform matrix if dirty.
	 * @param _forced If true, recompute even if not dirty
	 * @return True if recomputation occurred
	 */
	bool ApplyTransform(bool _forced = false) override;
	
	/**
	 * @brief Recomputes transform and all parent transforms.
	 * @return True if any recomputation occurred
	 */
	bool ApplyAllTransform() override;
	
	/**
	 * @brief Computes inverse transform matrix.
	 * @return True if inverse was recomputed
	 */
	bool GetInvTransform() const override;

	/**
	 * @brief Debug utility for transform state inspection.
	 * @return Debug status code
	 */
	int _debug() const override;

};