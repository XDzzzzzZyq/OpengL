/**
 * @file Camera.h
 * @brief Camera class providing view and projection matrices for rendering.
 * 
 * Camera is a scene object that defines viewpoint and projection for rendering.
 * It inherits from ObjectID (scene graph identity) and Transform3D (spatial placement).
 * Cameras compute view/projection matrices used by the Renderer to transform geometry.
 * 
 * Architecture:
 * 
 * - Owned by scene graph (shared_ptr in scene containers)
 * - Active camera selected via SceneContext
 * - Renderer reads camera matrices as immutable data
 * - Editor manipulates camera via CameraController events
 * 
 * @note Multiple cameras can exist, but only one is active at a time.
 */

#pragma once
#include "ID.h"
#include "Transform.h"
#include "DebugMesh.h"

#include <vector>

/*#include "Sprite.h"*/

/**
 * @brief Camera object defining viewpoint and projection for rendering.
 * 
 * Camera provides perspective projection with configurable FOV, aspect ratio,
 * and near/far clip planes. It also supports orthographic projection (TODO).
 * Transform3D provides position/rotation via standard scene graph interface.
 * 
 * Frustum matrices are cached and recomputed only when parameters change.
 * Camera data can be serialized to float array for GPU upload.
 * 
 * @note Inheritance: Inherits identity from ObjectID, transform from Transform3D.
 * @note Frustum culling: Camera frustum can be used for visibility tests (TODO: implement).
 */
class Camera : public ObjectID, public Transform3D
{
public:
	float cam_w;      ///< Viewport width in pixels
	float cam_h;      ///< Viewport height in pixels
	float cam_pers;   ///< Perspective field of view in degrees
	float cam_near;   ///< Near clipping plane distance
	float cam_far;    ///< Far clipping plane distance
	float cam_foc;    ///< Focal length (for depth of field effects)
	
	glm::vec3 cam_tar = glm::vec3(0.0f, 0.0f, 0.0f); ///< Look-at target position

	bool is_frustum_changed = true;          ///< Dirty flag indicating frustum needs recomputation
	glm::mat4 cam_frustum = glm::mat4(-1.0f); ///< Cached projection matrix

public:
	/**
	 * @brief Constructs a camera with explicit parameters.
	 * @param w Viewport width in pixels
	 * @param h Viewport height in pixels
	 * @param per Perspective FOV in degrees
	 * @param n Near clipping plane distance
	 * @param f Far clipping plane distance
	 */
	Camera(float w, float h, float per, float n, float f);
	
	/**
	 * @brief Constructs a camera with default parameters.
	 * @note Default values are defined in implementation.
	 */
	Camera();

public:
	// TODO: use std::array
	std::vector<float> cam_floatData; ///< Serialized camera data for GPU upload
	
	/**
	 * @brief Generates float array of camera parameters for GPU.
	 * 
	 * Serializes camera state into cam_floatData:
	 * - 6 floats: position (3) + rotation (3)
	 * - 1 float: aspect ratio (w/h)
	 * - 1 float: FOV angle in radians
	 * 
	 * @note Total size: 8 floats
	 */
	void GenFloatData();
	
	/**
	 * @brief Updates aspect ratio from width and height.
	 * @param w New viewport width
	 * @param h New viewport height
	 * @note Marks frustum as dirty for recomputation.
	 */
	void ChangeCamRatio(float w, float h);
	
	/**
	 * @brief Updates aspect ratio from 2D size vector.
	 * @param size Viewport size (width, height)
	 * @note Marks frustum as dirty for recomputation.
	 */
	void ChangeCamRatio(const glm::vec2& size);
	
	/**
	 * @brief Updates perspective field of view.
	 * @param persp New FOV in degrees
	 * @note Marks frustum as dirty for recomputation.
	 */
	void ChangeCamPersp(float persp);

	/**
	 * @brief Sets camera position in world space.
	 * @param _pos New camera position
	 * @note Updates Transform3D position.
	 */
	void SetCamPos(const glm::vec3& _pos);
	
	/**
	 * @brief Sets look-at target position.
	 * @param _pos Target position to look at
	 * @note Updates camera rotation to face target.
	 */
	void SetTarPos(const glm::vec3& _pos);
	
	/**
	 * @brief Sets camera transform from 4x4 matrix.
	 * @param _trans Transformation matrix
	 * @param pos Apply position component (default: true)
	 * @param rot Apply rotation component (default: true)
	 * @note Does not apply scale component (cameras ignore scale).
	 */
	void SetCamTrans(const glm::mat4& _trans, bool pos = true, bool rot = true);

	/**
	 * @brief Returns pointer to Transform component.
	 * @return Pointer to Transform3D as void* (cast required)
	 * @note Overrides ObjectID::GetTransform() for polymorphic access.
	 */
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }

public:
	/**
	 * @brief Destroys the camera.
	 */
	~Camera();
};

