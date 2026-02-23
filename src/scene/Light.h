/**
 * @file Light.h
 * @brief Light source objects for PBR lighting.
 *
 * Provides the Light class (point, sun, spot, area lights) with shadow mapping
 * support. GPU buffer management and shadow caching are handled by ShadowSystem
 * (src/render/ShadowSystem.h).
 *
 * Architecture:
 * - Light inherits ObjectID (scene identity) and Transform3D (spatial placement)
 * - ShadowSystem converts scene lights into GPU-friendly buffers
 * - Renderer reads light data via ShadowSystem bindings
 * - Shadow maps are lazily allocated and cached per-light
 *
 * @note TODO: Light should not own runtime properties like shadow maps (move to ShadowSystem).
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
 * @note TODO: Shadow map and projection matrix should move to ShadowSystem.
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
	Texture light_shadow_map;      ///< Shadow map texture (TODO: move to ShadowSystem)
	glm::mat4 light_proj{ 1.0f };  ///< Light projection matrix for shadow mapping (TODO: move to ShadowSystem)

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
	 * @note TODO: Move to ShadowSystem or shadow pipeline.
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

