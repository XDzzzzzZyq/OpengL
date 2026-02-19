/**
 * @file Light.h
 * @brief Light source objects and GPU buffer management for PBR lighting.
 * 
 * Provides Light class (point, sun, spot, area lights) and LightArrayBuffer for
 * efficient GPU upload of light data. Lights support shadow mapping with multiple
 * techniques (standard, moment-based, soft shadows).
 * 
 * Architecture:
 * - Light inherits ObjectID (scene identity) and Transform3D (spatial placement)
 * - LightArrayBuffer converts scene lights into GPU-friendly buffers
 * - Renderer reads light data via LightArrayBuffer bindings
 * - Shadow maps are lazily allocated and cached per-light
 * 
 * @note TODO: Light should not own runtime properties like shadow maps (move to LightArrayBuffer).
 * @note TODO: Move shadow configuration statics to RenderConfigs.
 */

#pragma once

#include "ID.h"
#include "Transform.h"
#include "Sprite.h"
#include "PolygonLight.h"

#include "buffer/FrameBuffer.h"
#include "buffer/StorageBuffer.h"
#include "buffer/UniformBuffer.h"

#include "RenderConfigs.h"

#include <array>

/**
 * @brief Enumeration of supported light types.
 */
enum LightType
{
	NONELIGHT = -1,  ///< Invalid or uninitialized light
	POINTLIGHT,      ///< Omnidirectional point light
	SUNLIGHT,        ///< Directional sun light (parallel rays)
	SPOTLIGHT,       ///< Cone-shaped spot light
	AREALIGHT        ///< Rectangular area light with soft shadows
};

/**
 * @brief Light source object supporting point, sun, spot, and area lights.
 * 
 * Light provides PBR lighting with configurable type, color, power, and shadows.
 * Transform3D determines position/orientation, while type-specific parameters
 * control light shape (radius for point, cutoff for spot, ratio for area).
 * 
 * Shadow Mapping:
 * - Each light can cast shadows via dedicated shadow map texture
 * - Shadow maps are allocated lazily on first use
 * - Supports standard and moment-based shadow techniques
 * - Projection matrix computed based on light type
 * 
 * @note Inheritance: ObjectID for scene identity, Transform3D for spatial transform.
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 * @note TODO: Shadow map and projection matrix should move to LightArrayBuffer.
 */
class Light : public ObjectID, public Transform3D
{
public:
	bool use_shadow{ true }; ///< Enable shadow casting for this light

	LightType light_type{ LightType::NONELIGHT }; ///< Type of light source
	float light_power{ 1.0f };                    ///< Luminous intensity (arbitrary units)
	glm::vec3 light_color{ 1.0f };                ///< RGB color multiplier (linear space)

	// Point light parameters
	float light_radius{ 0.05f }; ///< Physical radius for soft shadows

	// Spot light parameters
	float spot_cutoff{ 0.9f };       ///< Inner cone cosine (full brightness)
	float spot_outer_cutoff{ 0.8f }; ///< Outer cone cosine (falloff to zero)

	// Area light parameters
	float area_ratio{ 1.0f }; ///< Aspect ratio (width/height) of rectangular area light

public:
	Sprite light_sprite;           ///< Visual gizmo for editor viewport
	Texture light_shadow_map;      ///< Shadow map texture (TODO: move to LightArrayBuffer)
	glm::mat4 light_proj{ 1.0f };  ///< Light projection matrix for shadow mapping (TODO: move to LightArrayBuffer)

public:
	// TODO: Move to RenderConfigs
	static float sun_shaodow_field;  ///< Orthographic projection field size for sun shadows
	static float sun_shaodow_near;   ///< Near plane for sun shadow map
	static float sun_shaodow_far;    ///< Far plane for sun shadow map

	static float point_shaodow_near; ///< Near plane for point light shadow map
	static float point_shaodow_far;  ///< Far plane for point light shadow map
	static float point_blur_range;   ///< Blur kernel size for soft point shadows

	static float spot_shaodow_near;  ///< Near plane for spot light shadow map
	static float spot_shaodow_far;   ///< Far plane for spot light shadow map
	static float spot_blur_range;    ///< Blur kernel size for soft spot shadows

	static float area_shaodow_near;  ///< Near plane for area light shadow map
	static float area_shaodow_far;   ///< Far plane for area light shadow map
	static float area_blur_range;    ///< Blur kernel size for soft area shadows

public:
	/**
	 * @brief Constructs a light with default values.
	 */
	Light();
	
	/**
	 * @brief Constructs a light with specified type, power, and color.
	 * @param type Light type (POINTLIGHT, SUNLIGHT, SPOTLIGHT, AREALIGHT)
	 * @param power Luminous intensity (default: 10)
	 * @param color RGB color (default: white)
	 */
	Light(LightType type, float power = 10, glm::vec3 color = glm::vec3{ 1, 1, 1 });
	
	/**
	 * @brief Initializes shadow map texture for this light.
	 * @param using_moment_shadow Use moment-based shadow maps (variance/ESM) vs. standard depth
	 * @note Allocates GPU texture based on light type (2D for sun/spot, cubemap for point).
	 */
	void InitShadowMap(bool using_moment_shadow);
	
	/**
	 * @brief Parses light type to sprite icon and name.
	 * @param _type Light type to parse
	 * @return Pair of (sprite icon type, display name string)
	 * @note Used by editor to display light icons in viewport and outliner.
	 */
	inline static std::pair<SpriteType, std::string> ParseLightName(LightType _type);

public:
	bool is_light_changed{ false }; ///< Dirty flag indicating light parameters changed since last frame
	
	/**
	 * @brief Sets light color.
	 * @param _col New RGB color in linear space
	 */
	void SetColor(const glm::vec3& _col);
	
	/**
	 * @brief Sets light power/intensity.
	 * @param _power New luminous intensity
	 */
	void SetPower(float _power);
	
	/**
	 * @brief Enables or disables shadow casting.
	 * @param _state True to enable shadows, false to disable
	 */
	void SetShadow(bool _state);
	
	/**
	 * @brief Sets point light radius.
	 * @param _rad New radius for soft shadow computation
	 * @note Only applicable to POINTLIGHT type.
	 */
	void SetRadius(float _rad);
	
	/**
	 * @brief Sets spot light inner cutoff angle.
	 * @param _ang Cosine of inner cone angle
	 * @note Only applicable to SPOTLIGHT type.
	 */
	void SetCutoff(float _ang);
	
	/**
	 * @brief Sets spot light outer cutoff angle.
	 * @param _ang Cosine of outer cone angle (defines falloff region)
	 * @note Only applicable to SPOTLIGHT type.
	 */
	void SetOuterCutoff(float _ang);
	
	/**
	 * @brief Sets area light aspect ratio.
	 * @param _ratio Width/height ratio of rectangular area light
	 * @note Only applicable to AREALIGHT type.
	 */
	void SetRatio(float _ratio);

private:
	static std::array <FrameBuffer, 4> _shadowmap_buffer;  ///< Shared framebuffers for shadow rendering
	static std::array<ChainedShader, 4> _shadowmap_shader; ///< Shaders for shadow map generation
	static std::array<glm::mat4, 6> _point_6side;          ///< Six view matrices for point light cubemap

public:
	// TODO: Remove runtime construction
	/**
	 * @brief Initializes static shadow map resources (framebuffers, shaders).
	 * @note Called once during renderer initialization.
	 */
	static void EnableShadowMap();
	
	/**
	 * @brief Constructs Summed Area Table (SAT) for soft shadows.
	 * @param config Render configuration specifying shadow quality
	 * @note SAT enables efficient variable-size blur for soft shadow filtering.
	 * @note TODO: Move to LightArrayBuffer or shadow pipeline.
	 */
	void ConstructSAT(const RenderConfigs* config);

public:
	/**
	 * @brief Renders light sprite gizmo in viewport.
	 * @param ctx Context providing camera and viewport state
	 * @note Used by editor to display light icons (not part of scene rendering).
	 */
	void RenderLightSpr(const Context& ctx);

public:
	/**
	 * @brief Binds shadow map framebuffer for rendering.
	 * @note Sets render target to this light's shadow map texture.
	 */
	void BindShadowMapBuffer();
	
	/**
	 * @brief Binds shadow map generation shader.
	 * @note Shader writes depth or moment data to shadow map.
	 */
	void BindShadowMapShader();
	
	/**
	 * @brief Binds transform matrix for shadow-casting object.
	 * @param _trans Model matrix of geometry to render into shadow map
	 */
	void BindTargetTrans(const glm::mat4& _trans);
	
	/**
	 * @brief Updates light projection matrix based on current parameters.
	 * @note Projection type depends on light type (perspective for spot, ortho for sun, etc.).
	 */
	void UpdateProjMatrix();
	
	/**
	 * @brief Binds shadow map texture to shader slot.
	 * @param _slot OpenGL texture slot (default: GL_TEXTURE31)
	 * @note Used during lighting pass to sample shadow map.
	 */
	void BindShadowMap(GLuint _slot = GL_TEXTURE31);

public:
	/**
	 * @brief Returns pointer to Transform component.
	 * @return Pointer to Transform3D as void* (cast required)
	 */
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }
	
	/**
	 * @brief Returns pointer to shadow map shader.
	 * @return Pointer to ChainedShader as void*
	 */
	void* GetShader()		override;
};

/**
 * @brief Converts scene lights into GPU-friendly buffers for shader consumption.
 * 
 * LightArrayBuffer aggregates all lights in the scene and packs them into typed
 * storage buffers (SSBO) for efficient GPU access. Each light type has its own
 * buffer to enable type-specific shader processing.
 * 
 * GPU Layout:
 * - PointStruct: Point lights (position, color, power, radius, shadow flag)
 * - SunStruct: Directional sun lights (direction, projection matrix)
 * - SpotStruct: Spot lights (position, direction, cone angles)
 * - AreaStruct: Rectangular area lights (transform, aspect ratio)
 * - PolyStruct + PolyVertStruct: Polygonal area lights (vertex list)
 * 
 * Shadow Map Management:
 * - Caches shadow map textures per-light for efficient binding
 * - Handles shadow map resizing on viewport changes
 * - Binds shadow maps to dedicated texture slots (16-31)
 * 
 * @note All structs use explicit alignment (alignas) to match GLSL std140/std430 layout.
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 */
struct LightArrayBuffer {

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
		 * @brief Constructs SunStruct from Light object.
		 * @param light Source light (must be SUNLIGHT type)
		 */
		SunStruct(const Light& light);

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

	static const GLuint Sizeof_Point = sizeof(PointStruct);     ///< Size of PointStruct in bytes
	static const GLuint Sizeof_Sun   = sizeof(SunStruct);       ///< Size of SunStruct in bytes
	static const GLuint Sizeof_Spot  = sizeof(SpotStruct);      ///< Size of SpotStruct in bytes
	static const GLuint Sizeof_Area = sizeof(AreaStruct);       ///< Size of AreaStruct in bytes
	static const GLuint Sizeof_Poly  = sizeof(PolyStruct);      ///< Size of PolyStruct in bytes
	static const GLuint Sizeof_PolyVert  = sizeof(PolyVertStruct); ///< Size of PolyVertStruct in bytes

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

	mutable std::unordered_map<int, Texture> shadow_cache; ///< Shadow map texture cache (light ID -> texture)
	
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

public:
	/**
	 * @brief Constructs an empty LightArrayBuffer.
	 */
	LightArrayBuffer();
	
	/**
	 * @brief Destroys the LightArrayBuffer and releases GPU resources.
	 */
	~LightArrayBuffer();
	
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
	 * @note Populates point_list, sun_list, spot_list, area_list and uploads to GPU.
	 */
	void ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list);
	
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
	 * @brief Updates lighting cache and shadow maps for current frame.
	 * @param frame Current frame number
	 * @param config Render configuration for shadow quality
	 * @note Manages shadow map allocation and caching.
	 */
	void UpdateLightingCache(int frame, RenderConfigs* config);
	
	/**
	 * @brief Binds all shadow map textures to shader slots.
	 * @note Binds cached shadow maps to slots 16-31 for shader sampling.
	 */
	void BindShadowMap() const;
};
