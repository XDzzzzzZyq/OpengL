/**
 * @file Environment.h
 * @brief Environment object for skybox rendering and Image-Based Lighting (IBL).
 * 
 * Environment provides background rendering (skybox) and ambient lighting via
 * precomputed IBL textures (diffuse irradiance and specular prefiltered maps).
 * It inherits from ObjectID (scene identity) and Transform3D (orientation for
 * skybox rotation).
 * 
 * Architecture:
 * - Owned by scene graph (shared_ptr in Scene containers)
 * - Active environment selected via SceneContext
 * - Renderer reads IBL textures as immutable data for lighting pass
 * - Editor manipulates environment properties via events
 * 
 * @note Only one environment is active at a time (per scene).
 * @note IBL textures are precomputed offline or generated from HDR environment map.
 */

#pragma once

#include "ID.h"
#include "Transform.h"
#include "Texture.h"
#include "Sprite.h"

#include "Context.h"

#include "buffer/FrameBuffer.h"
#include "shaders/RenderShader.h"

/**
 * @brief Enumeration of supported environment types.
 */
enum EnvironmentType
{
	NONE_ENVIR,     ///< No environment (black background)
	TEXTURE_ENVIR,  ///< HDR texture skybox with IBL
	COLOR_ENVIR,    ///< Solid color background (no IBL)
	NOISE_ENVIR     ///< Procedural noise background
};

/**
 * @brief Environment object providing skybox and Image-Based Lighting.
 * 
 * Environment defines the scene's background and ambient lighting via IBL.
 * It supports HDR texture environments (equirectangular or cubemap), solid
 * color backgrounds, and procedural noise. IBL textures (diffuse irradiance
 * and specular prefiltered) are used in the PBR lighting model.
 * 
 * Resource Ownership:
 * - envir_IBL_diff: Diffuse irradiance cubemap (owned by Environment)
 * - envir_IBL_spec: Specular prefiltered cubemap (owned by Environment)
 * - envir_shader: Skybox rendering shader (owned by Environment)
 * 
 * Rendering Pipeline Integration:
 * - Background pass: Renders skybox as fullscreen quad at infinite depth
 * - IBL pass: Samples irradiance and prefiltered maps for ambient lighting
 * - Sprite rendering: Displays environment icon in editor viewport
 * 
 * @note Inheritance: ObjectID for scene identity, Transform3D for rotation.
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 * @note Gamma correction: envir_gamma controls skybox brightness adjustment.
 */
class Environment : public ObjectID, public Transform3D
{
public:

	/**
	 * @brief Constructs a default environment with no texture.
	 * @note Type defaults to NONE_ENVIR (black background).
	 */
	Environment();
	
	/**
	 * @brief Constructs an environment from HDR texture file.
	 * @param texpath Path to HDR environment map (equirectangular or cubemap)
	 * @note Automatically generates IBL textures from input map.
	 */
	Environment(const std::string& texpath);

public:

	RenderShader envir_shader; ///< Shader for rendering skybox

	Texture envir_IBL_diff;  ///< Diffuse irradiance cubemap (precomputed convolution)
	Texture envir_IBL_spec;  ///< Specular prefiltered cubemap (precomputed roughness mipmaps)
	EnvironmentType envir_type = EnvironmentType::NONE_ENVIR; ///< Active environment type
	bool use_envir = false;  ///< Enable environment rendering and IBL

	Sprite envir_sprite; ///< Visual gizmo for editor viewport

	float envir_gamma = 2.2f;              ///< Gamma correction for skybox display
	glm::vec3 envir_color = glm::vec3(1.0f); ///< Tint color for skybox (linear space)

public:

	/**
	 * @brief Changes environment texture.
	 * @param texpath Path to new HDR environment map
	 * @note Reloads IBL textures from new environment map.
	 */
	void ChangeEnvirTexture(const std::string& texpath) const;
	
	/**
	 * @brief Changes environment type.
	 * @param type New environment type (TEXTURE_ENVIR, COLOR_ENVIR, etc.)
	 * @note Switches rendering mode and IBL availability.
	 */
	void ChangeEnvirType(EnvironmentType type) const;

	/**
	 * @brief Binds environment framebuffer for rendering.
	 * @note Used for environment map generation or post-processing.
	 */
	void BindFrameBuffer() const;
	
	/**
	 * @brief Unbinds environment framebuffer.
	 */
	void UnbindFrameBuffer() const;
	
	/**
	 * @brief Swaps environment framebuffer attachments.
	 * @param type Framebuffer type to swap to
	 * @note Used for ping-pong rendering during IBL generation.
	 */
	void SwapFrameBuffer(FBType type);

	/**
	 * @brief Binds IBL textures to shader slots.
	 * @note Binds envir_IBL_diff and envir_IBL_spec for lighting shader access.
	 */
	void BindEnvironTexture() const;
	
	/**
	 * @brief Unbinds IBL textures.
	 */
	void UnbindEnvironTexture() const;

public:

	/**
	 * @brief Returns pointer to skybox shader.
	 * @return Pointer to RenderShader as void* (cast required)
	 * @note Overrides ObjectID::GetShader() for polymorphic access.
	 */
	void* GetShader()		override { return &envir_shader; }
	
	/**
	 * @brief Returns pointer to transform.
	 * @return Pointer to Transform3D as void* (cast required)
	 * @note Overrides ObjectID::GetTransform() for polymorphic access.
	 */
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }

	/**
	 * @brief Renders environment skybox to active framebuffer.
	 * @param ctx Context providing camera for view/projection matrices
	 * @note Renders fullscreen quad with skybox shader at infinite depth.
	 */
	void RenderEnvironment(const Context& ctx);
	
	/**
	 * @brief Renders environment sprite gizmo in viewport.
	 * @param ctx Context providing camera and viewport state
	 * @note Used by editor to display environment icon (not part of scene rendering).
	 */
	void RenderEnvirSpr(const Context& ctx);

};

