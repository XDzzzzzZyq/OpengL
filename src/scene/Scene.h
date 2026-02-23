/**
 * @file Scene.h
 * @brief Scene container holding all scene objects and scene-wide state.
 *
 * Provides the Scene class which aggregates cameras, lights, meshes,
 * environments, and other renderable objects into a unified scene graph.
 *
 * Architecture:
 * - Scene: Owns all scene objects via shared_ptr maps
 * - Context queries Scene to obtain active camera, environment, object lists
 * - Renderer reads Scene as immutable scene graph
 *
 * @note Scene objects are stored in type-specific maps (cam_list, mesh_list, etc.).
 * @note Active objects (camera, environment) are tracked separately for fast lookup.
 */

#pragma once

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "PolygonLight.h"
#include "Environment.h"
#include "DebugLine.h"
#include "DebugPoints.h"
#include "PostProcessing.h"

#include "SDFField.h"

/**
 * @brief Container for all scene objects and scene-wide state.
 *
 * Scene aggregates all renderable and non-renderable objects in a scene.
 * It provides typed pools (ResPool) for each object category, enabling efficient
 * iteration and lookup by ID. Active objects (camera, environment) are cached for
 * fast access during rendering.
 *
 * Scene Modification Tracking:
 * - SceneModifStatus flags track which aspects of the scene changed
 * - Renderer can optimize updates by checking status flags
 * - Flags reset after processing to avoid redundant work
 *
 * Object Lifetime:
 * - Objects owned via shared_ptr (reference counted)
 * - Removing from map triggers destruction if no other references exist
 * - GPU resources released in object destructors
 *
 * @note Inheritance: UID provides unique scene identifier.
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 */
class Scene : public UID
{
public:

	/**
	 * @brief Constructs an empty scene.
	 */
	Scene();

	/**
	 * @brief Destroys the scene and all owned objects.
	 * @note GPU resources released via object destructors.
	 */
	~Scene();

public:

	template<class Object>
	using Resource = std::shared_ptr<Object>; ///< Shared ownership wrapper for scene objects

	template<class Object>
	using ResPool = std::unordered_map<int, Resource<Object>>; ///< Object pool indexed by UID

	template<class Object>
	using ResList = std::vector<Resource<Object>>; ///< Ordered list of objects

public:

	/**
	 * @brief Scene modification status flags for change tracking.
	 */
	enum SceneModifStatus
	{
		NoChanges			= 0,                ///< No modifications since last check
		ObjectTransChanged	= 1 << 0,           ///< Object transforms updated
		LightChanged		= 1 << 1,           ///< Light properties changed
		CameraChanged		= 1 << 2,           ///< Camera properties changed
		ShaderChanged		= 1 << 3,           ///< Shaders recompiled
		MaterialChanged		= 1 << 4,           ///< Material properties changed

		SceneChanged		= ObjectTransChanged | LightChanged | CameraChanged | ShaderChanged | MaterialChanged, ///< Any scene content changed
		SDFChanged			= 1 << 8,           ///< SDF field requires rebuild
	};

	/**
	 * @brief Updates scene status flag (bitwise OR).
	 * @param tar Target status flag(s) to set
	 * @param value If true, set flag; if false, clear flag
	 * @note Multiple flags can be combined via bitwise OR.
	 */
	void UpdateSceneStatus(int tar, bool value);

	/**
	 * @brief Directly sets scene status (replaces existing).
	 * @param tar New status value
	 * @param value Unused (legacy parameter)
	 */
	void SetSceneStatus(int tar, bool value);

	/**
	 * @brief Checks if a status flag is set.
	 * @param tar Status flag(s) to check
	 * @return True if any of the specified flags are set
	 */
	bool CheckStatus(SceneModifStatus tar);

	/**
	 * @brief Prints current status flags to debug output.
	 * @note Development tool for debugging scene update logic.
	 */
	void _debugStatus();

private:
	SceneModifStatus status = SceneModifStatus::SceneChanged; ///< Current scene modification state

public:

	ResPool<ObjectID>			obj_list;         ///< All scene objects (base type)

	ResPool<Camera>				cam_list;         ///< Camera objects
	ResPool<Mesh>				mesh_list;        ///< Mesh objects
	ResPool<Light>				light_list;       ///< Light objects (point, sun, spot, area)
	ResPool<PolygonLight>		poly_light_list;  ///< Polygonal area lights
	ResPool<Environment>		envir_list;       ///< Environment objects (skybox, IBL)
	ResPool<Sprite>				sprite_list;      ///< 2D sprite overlays
	ResPool<DebugLine>			dLine_list;       ///< Debug line primitives
	ResPool<DebugPoints>		dPoints_list;     ///< Debug point primitives
	ResList<PostProcessing>		pps_list;         ///< Post-processing effect stack

	Resource<SDFField>          sdf_field;        ///< Signed Distance Field for soft shadows

public:

	/**
	 * @brief Registers a camera in the scene.
	 * @param camera Shared pointer to Camera object
	 * @note Adds to cam_list and obj_list.
	 */
	void UseCamera(Resource<Camera> camera);

	/**
	 * @brief Registers a mesh in the scene.
	 * @param mesh Shared pointer to Mesh object
	 * @note Adds to mesh_list and obj_list.
	 */
	void UseMesh(Resource<Mesh> mesh);

	/**
	 * @brief Registers a light in the scene.
	 * @param light Shared pointer to Light object
	 * @note Adds to light_list and obj_list.
	 */
	void UseLight(Resource<Light> light);

	/**
	 * @brief Registers a polygonal light in the scene.
	 * @param polyLight Shared pointer to PolygonLight object
	 * @note Adds to poly_light_list and obj_list.
	 */
	void UsePolygonLight(Resource<PolygonLight> polyLight);

	/**
	 * @brief Registers an environment in the scene.
	 * @param envir Shared pointer to Environment object
	 * @note Adds to envir_list and obj_list. Replaces active environment.
	 */
	void UseEnvironment(Resource<Environment> envir);

	/**
	 * @brief Registers debug lines in the scene.
	 * @param dline Shared pointer to DebugLine object
	 * @note Adds to dLine_list.
	 */
	void UseDebugLine(Resource<DebugLine> dline);

	/**
	 * @brief Registers debug points in the scene.
	 * @param dpoints Shared pointer to DebugPoints object
	 * @note Adds to dPoints_list.
	 */
	void UseDebugPoints(Resource<DebugPoints> dpoints);

	/**
	 * @brief Registers a post-processing effect.
	 * @param pps Shared pointer to PostProcessing object
	 * @note Appends to pps_list (order determines application sequence).
	 */
	void UsePostProcessing(Resource<PostProcessing> pps);

	/**
	 * @brief Assigns the scene's SDF field.
	 * @param sdf Shared pointer to SDFField object
	 * @note Replaces existing SDF field.
	 */
	void UseSDF(Resource<SDFField> sdf);

	/**
	 * @brief Retrieves object by UID.
	 * @param _id Unique object identifier
	 * @return Pointer to ObjectID, or nullptr if not found
	 */
	ObjectID* GetObjectID(int _id);

	/**
	 * @brief Returns the active camera.
	 * @return Pointer to active Camera, or nullptr if none
	 * @note First camera in cam_list is considered active (TODO: explicit active flag).
	 */
	Camera* GetActiveCamera();

	/**
	 * @brief Returns the active environment.
	 * @return Pointer to active Environment, or nullptr if none
	 * @note First environment in envir_list is considered active.
	 */
	Environment* GetActiveEnvironment();

	/**
	 * @brief Returns post-processing effect by index.
	 * @param _tar Effect index in pps_list
	 * @return Pointer to PostProcessing effect, or nullptr if out of range
	 */
	PostProcessing* GetPPS(int _tar);

public:

	/**
	 * @brief Updates all object transforms in hierarchy.
	 * @note Propagates parent transforms to children recursively.
	 */
	void UpdateObjTransforms();

	/**
	 * @brief Resets scene status to NoChanges.
	 * @note Called after Renderer processes all scene updates.
	 */
	void ResetStatus();
};
