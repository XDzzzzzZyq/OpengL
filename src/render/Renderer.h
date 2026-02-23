/**
 * @file Renderer.h
 * @brief Core renderer providing stateless rendering service.
 * 
 * The Renderer is a pure rendering service that consumes immutable Scene data
 * via Context and produces rendered frames. It owns GPU resources (buffers,
 * textures, shaders) but must not mutate application-level state.
 * 
 * Architecture:
 * - Receives const Context& (read-only scene access)
 * - Reads RenderConfigs for user settings
 * - Outputs to FrameBuffer for display
 * - Must not depend on Editor or UI layers
 * 
 * @note The Renderer is undergoing refactoring to achieve true statelessness.
 *       Many members marked with TODO should be moved to RenderContext or removed.
 */

#pragma once

#include "buffer/FrameBuffer.h"
#include "ShadowSystem.h"

#include "RenderConfigs.h"
#include "Context.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>

/**
 * @brief OpenGL 4.6 renderer providing real-time rendering pipeline.
 * 
 * Renderer executes a multi-pass deferred rendering pipeline with PBR lighting,
 * IBL, shadows, post-processing, and anti-aliasing. It manages GPU resources
 * (framebuffers, shaders, buffers) and ensures deterministic cleanup.
 * 
 * Rendering Pipeline:
 * 1. Geometry pass (G-Buffer: position, normal, albedo, metallic/roughness)
 * 2. Lighting pass (PBR evaluation with multi-light support)
 * 3. IBL pass (environment maps for diffuse/specular)
 * 4. Post-processing (SSAO, SSR, shadows, FXAA)
 * 5. Composite to final framebuffer
 * 
 * @note Ownership: Renderer owns all GPU resources and releases them in destructor.
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 */
class Renderer
{
private:
	/**
	 * @brief Built-in post-processing stack IDs.
	 */
	enum _BuildinPPS
	{
		_PBR_COMP_PPS,      ///< PBR composition pass
		_EDIT_VISUAL_PPS    ///< Editor visualization pass
	};

	/**
	 * @brief Built-in buffer location IDs.
	 */
	enum _BuildinBuffLoc
	{
		_RASTER,  ///< Rasterization buffer
		_AO_ELS   ///< Ambient occlusion elements buffer
	};

public:
	static GLint max_resolution_w; ///< Maximum supported framebuffer width
	static GLint max_resolution_h; ///< Maximum supported framebuffer height

public:

	// TODO: remove for stateless renderer
	GLuint r_frame_width{};  ///< Current frame width (to be moved to RenderContext)
	GLuint r_frame_height{}; ///< Current frame height (to be moved to RenderContext)

private:
	/**
	 * @brief The sampling step of progressive rendering.
	*/
	int r_sample_step{ 1 };

private:

	// TODO: remove for stateless renderer
	std::shared_ptr<FrameBuffer> r_render_result; ///< Main render output (to be externalized)
	
	/**
	 * @brief Resizes the rendering frame.
	 * @param _w New width in pixels
	 * @param _h New height in pixels
	 */
	void FrameResize(GLuint _w, GLuint _h);
	
	/**
	 * @brief Resizes framebuffers to match new dimensions.
	 * @param size New framebuffer size (width, height)
	 */
	void FrameBufferResize(const glm::vec2& size);

public:

	// TODO: remove for stateless renderer
	std::vector<FrameBuffer> r_buffer_list; ///< List of intermediate framebuffers (to be centralized)
	
	/**
	 * @brief Initializes all framebuffers for the rendering pipeline.
	 * @note Called during Renderer initialization
	 */
	void InitFrameBuffer();
	
	/**
	 * @brief Binds a framebuffer at the specified slot.
	 * @param slot Framebuffer slot index
	 */
	void BindFrameBuffer(int slot);
	
	/**
	 * @brief Unbinds a framebuffer at the specified slot.
	 * @param slot Framebuffer slot index
	 */
	void EndFrameBuffer(int slot);
	
	/**
	 * @brief Returns pointer to the main render result framebuffer.
	 * @return Non-owning pointer to result FrameBuffer
	 */
	FrameBuffer* GetFrameBufferPtr() { return r_render_result.get(); }

public:

	/**
	 * @brief Constructs a Renderer instance.
	 * @note Does not initialize GPU resources. Call Init() after construction.
	 */
	Renderer();
	
	/**
	 * @brief Initializes the renderer and subscribes to events.
	 * @param evt EventPool for subscribing to viewport resize and other events
	 * @note Must be called before Render()
	 */
	void Init(EventPool& evt);

	/**
	 * @brief Destroys the renderer and releases all GPU resources.
	 * @note All OpenGL objects are deleted deterministically
	 */
	~Renderer();

public:

	// TODO: move to editor layer / RenderContext
	bool r_render_icons = true; ///< Whether to render UI icons (editor-specific)
	bool r_is_preview = true;   ///< Whether in preview mode (editor-specific)

public:

	// TODO: remove for stateless renderer
	ShadowSystem r_light_data; ///< Shadow system managing light GPU buffers and shadow map caching
	RenderConfigs r_config;        ///< Render configuration (should use RenderContext)
	
	/**
	 * @brief Returns pointer to render configuration.
	 * @return Non-owning pointer to RenderConfigs
	 */
	RenderConfigs* GetConfig() { return &r_config; }

public:

	/**
	 * @brief Prepares for a new frame by clearing buffers.
	 * @note Call before Render() each frame
	 */
	void NewFrame();
	
	/**
	 * @brief Executes the full rendering pipeline.
	 * 
	 * Renders the scene from the active camera using multi-pass deferred rendering.
	 * Reads immutable scene data from Context and applies RenderConfigs settings.
	 * 
	 * @param ctx Context providing read-only scene access
	 * @param rend Enable rasterization pass (default: true)
	 * @param buff Enable buffer outputs (default: true)
	 * 
	 * @note This is the primary rendering entry point
	 * @note Context must not be mutated during or after this call
	 */
	void Render(const Context& ctx, bool rend = true, bool buff = true);

public:

	/**
	 * @brief Resets renderer state to initial configuration.
	 * @note Useful for clearing accumulated state between scenes
	 */
	void Reset();

	/**
	 * @brief Constructs SDF (Signed Distance Field) for soft shadows.
	 * 
	 * Builds a volumetric SDF from scene geometry for use in SDF-based
	 * soft shadow algorithms.
	 * 
	 * @param ctx Context providing scene geometry
	 * @note Performance: SDF construction is expensive. Cache when possible.
	 */
	void ConstructSDF(const Context& ctx);

public:

	/**
	 * @brief Saves the current frame to a screenshot file.
	 * @note File format and path are determined internally
	 */
	void ScreenShot();

};

