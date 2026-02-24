/**
 * @file FrameBuffer.h
 * @brief OpenGL framebuffer abstraction with multi-target support.
 * 
 * FrameBuffer provides RAII wrapper around OpenGL FBO (Framebuffer Object) with
 * support for multiple color attachments, depth buffers, and dynamic resizing.
 * Used extensively in deferred rendering for G-Buffer and post-processing.
 * 
 * @note All OpenGL resources are released deterministically in destructor.
 */

#pragma once

#include "RenderBuffer.h"

#include <cstdarg>
#include <optional>
#include <unordered_map>
#include <vector>


/**
 * @brief Framebuffer attachment type identifiers.
 * 
 * Each FBType corresponds to a specific framebuffer attachment used in the
 * deferred rendering pipeline. Types are organized by format (HDR, RGB, Float).
 */
enum FBType
{
	/* nothing */	NONE_FB = -1,      ///< Invalid/null framebuffer
	/*     HDR */	COMBINE_FB = 0,    ///< Combined final output (HDR)
					EMIS_COL_FB,       ///< Emission + color (HDR)
					LIGHT_AO_FB = 1,   ///< Lighting + AO (HDR)
					DIR_DIFF_FB = 1,   ///< Direct diffuse (HDR)
					POS_FB,            ///< World-space position (HDR)
					POS_B_FB = 2,      ///< Position buffer B
					DIR_SPEC_FB = 2,   ///< Direct specular (HDR)
					MRSE_FB,           ///< Metalness, roughness, specular, emission
					OPT_FLW_FB = 3,    ///< Optical flow motion vectors
					IND_DIFF_FB = 3,   ///< Indirect diffuse (HDR)
					NORMAL_FB,         ///< View-space normals
					IND_SPEC_FB = 4,   ///< Indirect specular (HDR)
					DIR_EMIS_FB = 5,   ///< Direct emission (HDR)
	/*     RGB */	ALBEDO_FB,         ///< Albedo color (RGB)
					MASK_FB,           ///< Masking buffer
					RAND_FB,           ///< Random noise buffer
					ID_FB,             ///< Object ID buffer
	/*   Float */	SINGLE_FB,         ///< Single-channel float buffer
					SHADOW_FB,         ///< Shadow map buffer
					MAX_FB,            ///< Maximum framebuffer type
	/*   Depth */	DEPTH_FB = GL_DEPTH_ATTACHMENT - GL_COLOR_ATTACHMENT0 ///< Depth attachment
};

/**
 * @brief Available G-Buffer passes for deferred rendering.
 */
#define AVAIL_PASSES { EMIS_COL_FB, POS_FB, NORMAL_FB, ALBEDO_FB, MRSE_FB, RAND_FB, ID_FB, MASK_FB }

/**
 * @brief Result passes for lighting composition.
 */
#define RESULT_PASSES { COMBINE_FB, DIR_DIFF_FB, DIR_SPEC_FB, IND_DIFF_FB, IND_SPEC_FB, DIR_EMIS_FB }

/**
 * @brief Framebuffer pixel data structure.
 */
struct FBPixel
{
	float RGBA[4]; ///< Pixel color components
	
	/**
	 * @brief Extracts integer ID from first component (for ID buffers).
	 * @return Object ID stored in R channel
	 */
	int GetID() const { return (int)RGBA[0]; }
};

/**
 * @brief OpenGL framebuffer wrapper with multi-target and dynamic resize support.
 * 
 * FrameBuffer encapsulates OpenGL FBO with multiple color attachments and optional
 * depth buffer. Supports deferred rendering by binding multiple targets simultaneously.
 * 
 * Features:
 * - Multiple color attachments (up to GL_MAX_COLOR_ATTACHMENTS)
 * - Optional depth/stencil via RenderBuffer
 * - Dynamic resizing of all attachments
 * - Texture binding for shader sampling
 * - Pixel readback for picking
 * 
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 * @note Ownership: Owns OpenGL FBO and textures. Cleans up in destructor.
 */
class FrameBuffer
{
private:
	GLuint fb_ID = 0;       ///< OpenGL framebuffer object ID
	GLuint fb_attach = 0;   ///< Attachment configuration flags
	GLuint fb_w{};          ///< Framebuffer width in pixels
	GLuint fb_h{};          ///< Framebuffer height in pixels
	
	/**
	 * @brief Maps FBType to texture index in fb_tex_list.
	 */
	mutable std::unordered_map<FBType, int> fb_type_list;
	
	/**
	 * @brief Converts FBType to OpenGL texture type.
	 * @param _type Framebuffer type
	 * @return Corresponding Texture::TextureType
	 */
	static Texture::TextureType PareseTexType(FBType _type);

	void _cpyInfo(const FrameBuffer& fb);     ///< Copies scalar metadata (dimensions, attach flags)
	void _delFB();                             ///< Deletes OpenGL framebuffer

	/**
	 * @brief Creates a new FBO that is an independent deep copy of @p fb.
	 *
	 * Steps performed:
	 * 1. Copies scalar metadata via _cpyInfo.
	 * 2. Deep-copies all textures (each Texture gets its own GL object).
	 * 3. Deep-copies the optional RenderBuffer (new GL renderbuffer same format/size).
	 * 4. Generates a new FBO and re-attaches all textures and renderbuffer.
	 * 5. Restores draw-buffer and read-buffer state.
	 *
	 * @param fb Source framebuffer.
	 */
	void _deepCopyFrom(const FrameBuffer& fb);
	
	/**
	 * @brief Resets framebuffer ID, deleting old FBO if different.
	 * @param _ID New framebuffer ID
	 */
	void _resetFBID(GLuint _ID) { if (fb_ID > 0 && fb_ID != _ID)_delFB(); fb_ID = _ID; }

public:
	std::optional<RenderBuffer> renderBuffer; ///< Optional depth/stencil renderbuffer
	mutable std::vector<Texture> fb_tex_list; ///< Textures for color attachments

public:

	/**
	 * @brief Default constructor (empty framebuffer).
	 */
	FrameBuffer();
	
	/**
	 * @brief Constructs a single-target framebuffer.
	 * @param type Framebuffer type for the single attachment
	 * @param attach Attachment index (default: 0)
	 */
	FrameBuffer(FBType type, GLuint attach=0);
	
	/**
	 * @brief Constructs a multi-target framebuffer (compile-time list).
	 * @param count Number of framebuffer types
	 * @param ... Variadic FBType arguments
	 */
	FrameBuffer(int count, ...);
	
	/**
	 * @brief Constructs a multi-target framebuffer (runtime list).
	 * @param _tars Vector of FBType targets
	 */
	FrameBuffer(const std::vector<FBType>& _tars);
	
	/**
	 * @brief Constructs a depth-only framebuffer.
	 * @param _depth Depth texture (moved into framebuffer)
	 */
	FrameBuffer(Texture&& _depth);
	
	/**
	 * @brief Copy constructor. Performs a GPU-side deep copy.
	 *
	 * Allocates a new FBO and deep-copies all attached textures and the
	 * optional depth/stencil renderbuffer. The resulting FrameBuffer is
	 * fully independent of the source.
	 *
	 * @param fb Source framebuffer.
	 */
	FrameBuffer(const FrameBuffer& fb);
	
	/**
	 * @brief Move constructor (transfers ownership).
	 * @param fb Source framebuffer (invalidated)
	 */
	FrameBuffer(FrameBuffer&& fb) noexcept;

	/**
	 * @brief Copy assignment operator.
	 * @param fb Source framebuffer
	 * @return Reference to this
	 */
	FrameBuffer& operator=(const FrameBuffer& fb);
	
	/**
	 * @brief Move assignment operator.
	 * @param fb Source framebuffer (invalidated)
	 * @return Reference to this
	 */
	FrameBuffer& operator=(FrameBuffer&& fb) noexcept;
	
	/**
	 * @brief Destructor. Releases all OpenGL resources.
	 */
	~FrameBuffer();

	/**
	 * @brief Binds this framebuffer for rendering.
	 * @note Subsequent draw calls render to this FBO
	 */
	void BindFrameBuffer() const;
	
	/**
	 * @brief Unbinds the current framebuffer (binds default FBO 0).
	 */
	static void UnbindFrameBuffer();

	/**
	 * @brief Links an existing texture to this framebuffer.
	 * @param _tex Texture to link
	 */
	void LinkTexture(const Texture& _tex);
	
	/**
	 * @brief Appends a new texture attachment.
	 * @param _tex Texture to append
	 * @param _type FBType for this attachment
	 */
	void AppendTexture(const Texture& _tex, FBType _type);

public:

	/**
	 * @brief Resizes all or selected attachments.
	 * @param size New size (width, height)
	 * @param all If true, resize all textures; otherwise only depth
	 */
	void Resize(const glm::vec2& size, bool all = false);
	
	/**
	 * @brief Resizes all or selected attachments.
	 * @param w New width in pixels
	 * @param h New height in pixels
	 * @param all If true, resize all textures; otherwise only depth
	 */
	void Resize(GLuint w, GLuint h, bool all = false);
	
	/**
	 * @brief Returns current framebuffer dimensions.
	 * @return Size (width, height)
	 */
	glm::vec2 GetSize() const { return { fb_w, fb_h }; }

	/**
	 * @brief Reads a pixel from the specified attachment.
	 * @param x Pixel x coordinate
	 * @param y Pixel y coordinate
	 * @param type FBType to read from
	 * @return Pixel data (RGBA float)
	 * @note Used for object picking via ID buffer
	 */
	FBPixel ReadPix(GLuint x, GLuint y, FBType type) const;

public:

	/**
	 * @brief Binds multiple framebuffer textures for shader sampling (variadic).
	 * @param count Number of FBType arguments
	 * @param ... Variadic FBType arguments
	 */
	void BindFrameBufferTex(int count = 0, ...) const;
	
	/**
	 * @brief Binds multiple framebuffer textures for shader sampling (vector).
	 * @param _tars Vector of FBType to bind
	 */
	void BindFrameBufferTex(const std::vector<FBType>& _tars) const;
	
	/**
	 * @brief Binds a single framebuffer texture to a shader slot.
	 * @param tar FBType to bind
	 * @param slot Texture unit (GL_TEXTURE0 + slot)
	 */
	void BindFrameBufferTex(FBType tar, GLuint slot) const;
	
	/**
	 * @brief Binds a single framebuffer texture for reading.
	 * @param tar FBType to bind
	 * @param slot Texture unit
	 */
	void BindFrameBufferTexR(FBType tar, GLuint slot) const;
	
	/**
	 * @brief Unbinds a framebuffer texture from reading.
	 * @param tar FBType to unbind
	 * @param slot Texture unit
	 */
	void UnbindFrameBufferTexR(FBType tar, GLuint slot) const;
	
	/**
	 * @brief Unbinds all framebuffer textures.
	 */
	void UnbindFrameBufferTex() const;

public:

	/**
	 * @brief Returns OpenGL framebuffer object ID.
	 * @return FBO ID
	 */
	GLuint GetFrameBufferID() const { return fb_ID; }
	
	/**
	 * @brief Returns framebuffer dimensions.
	 * @return Size (width, height)
	 */
	glm::vec2 GetFrameBufferSize() const { return { fb_w, fb_h }; }
	
	/**
	 * @brief Returns OpenGL texture ID for a given type.
	 * @param type FBType to query
	 * @return Texture ID
	 */
	GLuint GetFBTextureID(FBType type) const { return fb_tex_list[fb_type_list[type]].GetTexID(); }
	
	/**
	 * @brief Returns pointer to texture for a given type.
	 * @param type FBType to query
	 * @return Non-owning pointer to Texture
	 */
	Texture* GetFBTexturePtr(FBType type) const { return &fb_tex_list[fb_type_list[type]]; }
	
	/**
	 * @brief Returns number of color attachments.
	 * @return Attachment count
	 */
	size_t GetFBCount() const { return fb_tex_list.size(); }
	
	/**
	 * @brief Returns attachment location for a given type.
	 * @param type FBType to query
	 * @return Attachment index
	 */
	GLuint GetAttachmentLoc(FBType type) const { return fb_type_list[(FBType)type]; }
};

