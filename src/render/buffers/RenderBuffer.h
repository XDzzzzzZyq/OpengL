#pragma once
#include "Texture.h"

/**
 * @brief OpenGL renderbuffer object wrapper with RAII semantics.
 *
 * RenderBuffer encapsulates a GL renderbuffer (glGenRenderbuffers) used as
 * a depth/stencil attachment in FrameBuffer. Tracks the storage format and
 * dimensions so that copy semantics can recreate an equivalent renderbuffer
 * without sharing the same GL object ID.
 *
 * Copy semantics (deep copy):
 * - A new renderbuffer is allocated with the same format and current size.
 *
 * Move semantics:
 * - Ownership is transferred; source rb_ID is zeroed.
 *
 * @note GPU Resource Ownership: Owns rb_ID, released via glDeleteRenderbuffers
 *       in destructor.
 */
class RenderBuffer
{
private:
	GLuint rb_ID     = 0;                      ///< OpenGL renderbuffer object ID
	GLuint rb_format = GL_DEPTH24_STENCIL8;    ///< Storage format (for copy and resize)
	GLuint rb_w      = 0;                      ///< Current width in pixels
	GLuint rb_h      = 0;                      ///< Current height in pixels

	void _delRB();

	/**
	 * @brief Creates a new renderbuffer with the same format and dimensions as @p rb.
	 * @param rb Source renderbuffer to copy from.
	 */
	void _deepCopyFrom(const RenderBuffer& rb);

public:

	RenderBuffer();
	RenderBuffer(GLuint _type);
	~RenderBuffer();

	/**
	 * @brief Copy constructor. Allocates a new renderbuffer with the same format/size.
	 * @param rb Source renderbuffer.
	 */
	RenderBuffer(const RenderBuffer& rb);

	/**
	 * @brief Move constructor. Transfers ownership; @p rb is left empty.
	 * @param rb Source renderbuffer (invalidated after move).
	 */
	RenderBuffer(RenderBuffer&& rb) noexcept;

	/**
	 * @brief Copy assignment. Allocates a new renderbuffer with the same format/size.
	 * @param rb Source renderbuffer.
	 * @return Reference to this.
	 */
	RenderBuffer& operator=(const RenderBuffer& rb);

	/**
	 * @brief Move assignment. Transfers ownership; @p rb is left empty.
	 * @param rb Source renderbuffer (invalidated after move).
	 * @return Reference to this.
	 */
	RenderBuffer& operator=(RenderBuffer&& rb) noexcept;

public:

	void BindRenderBuffer() const;
	void UnbindRenderBuffer() const;

	void Resize(GLuint w, GLuint h);
	void Resize(const glm::vec2& size);

	GLuint GetRenderBufferID() const { return rb_ID; }
};
