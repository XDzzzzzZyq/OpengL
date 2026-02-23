/**
 * @file ShadowSystem.h
 * @brief GPU light buffer management, shadow map caching, and shadow computation.
 *
 * Provides ShadowSystem for aggregating scene lights into typed GPU storage
 * buffers (SSBO), owning per-light shadow map textures and projection matrices,
 * and managing the shadow computation pipeline.
 *
 * Architecture:
 * - ShadowSystem converts scene lights into GPU-friendly buffers
 * - ShadowSystem owns per-light shadow maps (raw depth/moment textures)
 * - ShadowSystem owns per-light projection matrices
 * - Renderer reads light data via ShadowSystem bindings
 *
 * @note Separated from Light (scene layer) because ShadowSystem is a render-layer
 *       concern: it manages GPU resources and shadow computation, not scene objects.
 */

#pragma once

#include "Light.h"
#include "RenderConfigs.h"
#include "Texture.h"
#include "buffer/StorageBuffer.h"
#include "buffer/UniformBuffer.h"
#include "PolygonLight.h"

#include <unordered_map>
#include <vector>
#include <memory>

/**
 * @brief Owns per-light GPU resources, shadow maps, and shadow computation.
 *
 * ShadowSystem aggregates all lights in the scene and packs them into typed
 * storage buffers (SSBO) for efficient GPU access. It also owns the per-light
 * shadow map textures and projection matrices that were previously held by Light.
 *
 * GPU Layout:
 * - PointStruct: Point lights (position, color, power, radius, shadow flag)
 * - SunStruct: Directional sun lights (direction, projection matrix)
 * - SpotStruct: Spot lights (position, direction, cone angles)
 * - AreaStruct: Rectangular area lights (transform, aspect ratio)
 * - PolyStruct + PolyVertStruct: Polygonal area lights (vertex list)
 *
 * Shadow Map Management:
 * - Owns shadow map textures per-light (raw depth / moment maps)
 * - Owns projection matrices per-light
 * - Handles shadow map resizing on viewport changes
 * - Binds shadow maps to dedicated texture slots (16-31)
 *
 * @note All structs use explicit alignment (alignas) to match GLSL std140/std430 layout.
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 */
struct ShadowSystem {

public:
	// In GLSL (shader), the size of vec3 equals to vec4 due to std140 alignment
	// See: https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf#page=159

	/**
	 * @brief GPU layout for point light data.
	 */
	struct PointStruct
	{
		/**
		 * @brief Constructs PointStruct from Light object.
		 * @param light Source light (must be POINTLIGHT type)
		 */
		PointStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };   ///< RGB color in linear space
		alignas(16) glm::vec3 pos{ 0 };     ///< World position

		alignas(4) float power{ 1.0f };     ///< Luminous intensity
		alignas(4) int use_shadow{ 1 };     ///< Shadow enabled flag (bool as int)
		alignas(4) float radius{ 0.05f };   ///< Physical radius for soft shadows
	};

	/**
	 * @brief GPU layout for sun/directional light data.
	 */
	struct SunStruct
	{
		/**
		 * @brief Constructs SunStruct from Light object and its projection matrix.
		 * @param light Source light (must be SUNLIGHT type)
		 * @param proj Light-space projection matrix owned by ShadowSystem
		 */
		SunStruct(const Light& light, const glm::mat4& proj);

		alignas(16) glm::vec3 color{ 1 };         ///< RGB color in linear space
		alignas(16) glm::vec3 pos{ 0 };           ///< World position (for visualization)
		alignas(16) glm::vec3 dir{ 1, 0, 0 };     ///< Light direction (normalized)

		alignas(4) float power{ 1.0f };           ///< Luminous intensity
		alignas(4) int use_shadow{ 1 };           ///< Shadow enabled flag (bool as int)
		alignas(16) glm::mat4 proj_trans;         ///< Light-space projection matrix
	};

	/**
	 * @brief GPU layout for spot light data.
	 */
	struct SpotStruct
	{
		/**
		 * @brief Constructs SpotStruct from Light object.
		 * @param light Source light (must be SPOTLIGHT type)
		 */
		SpotStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };         ///< RGB color in linear space
		alignas(16) glm::vec3 pos{ 0 };           ///< World position
		alignas(16) glm::vec3 dir{ 1, 0, 0 };     ///< Spotlight direction (normalized)

		alignas(4) float power{ 1.0f };           ///< Luminous intensity
		alignas(4) int use_shadow{ 1 };           ///< Shadow enabled flag (bool as int)
		alignas(4) float cutoff{ 0.9f };          ///< Inner cone angle (cosine)
		alignas(4) float outer_cutoff{ 0.8f };    ///< Outer cone angle (cosine)
	};

	/**
	 * @brief GPU layout for area light data.
	 */
	struct AreaStruct
	{
		/**
		 * @brief Constructs AreaStruct from Light object.
		 * @param light Source light (must be AREALIGHT type)
		 */
		AreaStruct(const Light& light);

		alignas(16) glm::vec3 color{ 1 };         ///< RGB color in linear space
		alignas(16) glm::mat4 trans{ 1 };         ///< World transform matrix

		alignas(4) float power{ 1.0f };           ///< Luminous intensity
		alignas(4) int use_shadow{ 1 };           ///< Shadow enabled flag (bool as int)
		alignas(4) float ratio{ 1.0f };           ///< Width/height aspect ratio
	};

	/**
	 * @brief GPU layout for polygonal light header data.
	 */
	struct PolyStruct
	{
		alignas(16) glm::vec3 color{ 1 };         ///< RGB color in linear space

		alignas(4) float power{ 1.0f };           ///< Luminous intensity
		alignas(4) int use_shadow{ 1 };           ///< Shadow enabled flag (bool as int)
		alignas(4) int n{ 3 };                    ///< Number of vertices in polygon
	};

	/**
	 * @brief GPU layout for polygonal light vertex data.
	 */
	struct PolyVertStruct
	{
		alignas(16) glm::vec3 v{ 0, 0, 0 };       ///< Vertex position in world space
	};

	static const GLuint Sizeof_Point    = sizeof(PointStruct);    ///< Size of PointStruct in bytes
	static const GLuint Sizeof_Sun      = sizeof(SunStruct);      ///< Size of SunStruct in bytes
	static const GLuint Sizeof_Spot     = sizeof(SpotStruct);     ///< Size of SpotStruct in bytes
	static const GLuint Sizeof_Area     = sizeof(AreaStruct);     ///< Size of AreaStruct in bytes
	static const GLuint Sizeof_Poly     = sizeof(PolyStruct);     ///< Size of PolyStruct in bytes
	static const GLuint Sizeof_PolyVert = sizeof(PolyVertStruct); ///< Size of PolyVertStruct in bytes

	/**
	 * @brief Light count statistics for current scene.
	 */
	struct SceneInfo {
		int point_count{ 0 };       ///< Number of point lights
		int sun_count{ 0 };         ///< Number of sun lights
		int spot_count{ 0 };        ///< Number of spot lights
		int area_count{ 0 };        ///< Number of area lights
		int poly_count{ 0 };        ///< Number of polygonal lights
		int poly_verts_count{ 0 };  ///< Total vertices across all polygonal lights
	};

public:
	std::vector<PointStruct> point_list; ///< List of point lights for GPU upload
	std::vector<SunStruct> sun_list;     ///< List of sun lights for GPU upload
	std::vector<SpotStruct> spot_list;   ///< List of spot lights for GPU upload
	std::vector<AreaStruct> area_list;   ///< List of area lights for GPU upload
	std::vector<PolyStruct> poly_list;   ///< List of polygonal light headers for GPU upload
	std::vector<PolyVertStruct> poly_verts; ///< List of polygonal light vertices for GPU upload

	mutable std::unordered_map<int, Texture> shadow_cache;   ///< Processed shadow cache per light (light ID -> texture)
	std::unordered_map<int, Texture> shadow_maps;            ///< Raw per-light shadow map textures (depth / moment)
	std::unordered_map<int, glm::mat4> proj_matrices;        ///< Per-light projection matrices for shadow rendering

	StorageBuffer point_buffer;      ///< GPU buffer for point light data
	StorageBuffer sun_buffer;        ///< GPU buffer for sun light data
	StorageBuffer spot_buffer;       ///< GPU buffer for spot light data
	StorageBuffer area_buffer;       ///< GPU buffer for area light data
	StorageBuffer poly_buffer;       ///< GPU buffer for polygonal light header data
	StorageBuffer poly_verts_buffer; ///< GPU buffer for polygonal light vertex data

	UniformBuffer info; ///< GPU buffer for SceneInfo metadata

private:
	using _LightInfo = std::tuple<int, Light*>;                      ///< Cache entry: (slot index, light pointer)
	std::unordered_map<int, _LightInfo> light_info_cache;            ///< Maps light ID to slot and pointer
	GLuint cache_w{};                                                ///< Cached shadow map width
	GLuint cache_h{};                                                ///< Cached shadow map height
	bool prev_moment_shadow{ false };                                ///< Tracks previous moment shadow mode for format-change detection

public:
	/**
	 * @brief Constructs an empty ShadowSystem.
	 */
	ShadowSystem();

	/**
	 * @brief Destroys the ShadowSystem and releases GPU resources.
	 */
	~ShadowSystem();

	/**
	 * @brief Initializes GPU buffers.
	 * @note Must be called after OpenGL context creation.
	 */
	void Init();

	/**
	 * @brief Binds all light buffers to shader binding points.
	 * @note Called before rendering to make light data accessible to shaders.
	 */
	void Bind() const;

public:
	static constexpr int shadow_slot[16]{31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16}; ///< Texture slots for shadow maps (GL_TEXTURE16-31)

public:
	/**
	 * @brief Parses basic lights (point, sun, spot, area) into GPU buffers.
	 * @param light_list Map of light ID to Light shared_ptr
	 * @param using_moment_shadow Initialize shadow maps with moment-based (HDR) format
	 * @note Populates point_list, sun_list, spot_list, area_list and uploads to GPU.
	 * @note Also initializes shadow_maps and proj_matrices for each light.
	 */
	void ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list, bool using_moment_shadow = false);

	/**
	 * @brief Parses polygonal lights into GPU buffers.
	 * @param poly_light_list Map of polygon light ID to PolygonLight shared_ptr
	 * @note Populates poly_list and poly_verts, then uploads to GPU.
	 */
	void ParsePolygonLightData(const std::unordered_map<int, std::shared_ptr<PolygonLight>>& poly_light_list);

public:
	/**
	 * @brief Returns light count statistics for current scene.
	 * @return SceneInfo struct with counts per light type
	 */
	SceneInfo GetSceneInfo() const;

	/**
	 * @brief Returns total number of lights across all types.
	 * @return Sum of all light counts (point + sun + spot + area + poly)
	 */
	GLsizei GetTotalCount() const;

	/**
	 * @brief Returns shadow map slot offset for a given light type.
	 * @param _type Light type (POINTLIGHT, SUNLIGHT, etc.)
	 * @return Starting texture slot index for this light type
	 */
	GLuint GetSlotOffset(LightType _type) const;

	/**
	 * @brief Resizes all cached shadow maps.
	 * @param _w New shadow map width
	 * @param _h New shadow map height
	 * @note Called on viewport resize to match shadow map resolution.
	 */
	void Resize(GLuint _w, GLuint _h);

public:
	/**
	 * @brief Updates a single light's GPU data.
	 * @param light Pointer to modified Light object
	 * @note Incrementally updates GPU buffer without full re-upload.
	 */
	void UpdateLight(Light* light);

	/**
	 * @brief Updates shadow maps for the current frame.
	 * @param frame Current frame number
	 * @param config Render configuration for shadow quality
	 * @note Manages per-light shadow cache updates and format changes.
	 */
	void Update(int frame, RenderConfigs* config);

	/**
	 * @brief Binds all shadow map textures to shader slots.
	 * @note Binds cached shadow maps to slots 16-31 for shader sampling.
	 */
	void BindShadowMap() const;

public:
	/**
	 * @brief Initializes the raw shadow map texture for a single light.
	 * @param light Light to initialize shadow map for
	 * @param using_moment_shadow Use moment-based (HDR) format instead of depth
	 * @note Allocates GPU texture based on light type (2D for sun/spot, cubemap for point).
	 */
	void InitShadowMap(Light* light, bool using_moment_shadow);

	/**
	 * @brief Computes and stores the projection matrix for a light.
	 * @param light Light whose projection matrix should be updated
	 * @note Result is stored in proj_matrices keyed by light ID.
	 */
	void UpdateProjMatrix(Light* light);

	/**
	 * @brief Constructs Summed Area Table (SAT) for soft shadows.
	 * @param light Light whose shadow map should be processed
	 * @param config Render configuration specifying shadow quality
	 * @note SAT enables efficient variable-size blur for soft shadow filtering.
	 */
	void ConstructSAT(Light* light, const RenderConfigs* config);
};

