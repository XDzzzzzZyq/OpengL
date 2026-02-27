/**
 * @file Buffers.h
 * @brief RAII base class for OpenGL buffer objects.
 *
 * Buffers owns a single OpenGL buffer object created via glGenBuffers and
 * provides canonical copy, move, and deep-copy semantics. Derived classes
 * (VertexBuffer, IndexBuffer, UniformBuffer, StorageBuffer) specialise the
 * binding target and extend with type-specific upload/download helpers.
 *
 * GPU Resource Ownership:
 * - Buffers owns buf_ID; released via glDeleteBuffers in destructor.
 * - Move transfers ownership; source buf_ID is zeroed.
 * - Copy creates a new GPU buffer with identical content (deep copy).
 * - Deep copy uses glCopyBufferSubData; no CPU round-trip is required.
 *
 * Architecture:
 * - This is a protected base; instantiate derived types only.
 * - buf_size tracks the allocated byte count so deep copy knows how much to copy.
 * - Derived constructors must update buf_size whenever glBufferData is called.
 */

#pragma once

#include <GL/glew.h>

/**
 * @brief RAII base class for OpenGL buffer objects (GL_ARRAY_BUFFER family).
 *
 * Encapsulates a single OpenGL buffer object ID (buf_ID) and its allocated
 * size (buf_size). Provides correct copy and move semantics so that derived
 * buffer classes inherit safe resource management without duplicating boilerplate.
 *
 * Copy semantics (deep copy):
 * @code
 * VertexBuffer a(data);
 * VertexBuffer b = a;   // new GPU buffer allocated; content copied via glCopyBufferSubData
 * @endcode
 *
 * Move semantics (ownership transfer):
 * @code
 * VertexBuffer a(data);
 * VertexBuffer b = std::move(a);  // b owns the buffer; a.GetID() == 0
 * @endcode
 *
 * @note Thread-safety: Not thread-safe. Must be used from the OpenGL context thread.
 * @note Only GL buffer objects (glGenBuffers) are managed here.
 *       Renderbuffers and framebuffers have separate RAII wrappers.
 */
class Buffers
{
protected:
	GLuint buf_ID   = 0; ///< OpenGL buffer object ID (0 = invalid/empty)
	GLuint buf_size = 0; ///< Allocated buffer size in bytes

	/**
	 * @brief Deletes the OpenGL buffer object and resets buf_ID to 0.
	 */
	void _delBuffer();

	/**
	 * @brief GPU-side deep copy from @p src.
	 *
	 * Allocates a new buffer of the same size and copies all content using
	 * glCopyBufferSubData (GL_COPY_READ_BUFFER → GL_COPY_WRITE_BUFFER).
	 * If @p src is empty (buf_ID == 0 or buf_size == 0) only the metadata
	 * is copied; no GPU allocation is performed.
	 *
	 * @param src Source buffer to copy from.
	 */
	void _deepCopyFrom(const Buffers& src);

public:
	Buffers() = default;

	/**
	 * @brief Destructor. Releases the OpenGL buffer object if valid.
	 */
	~Buffers();

	/**
	 * @brief Copy constructor. Performs a GPU-side deep copy of @p buf.
	 * @param buf Source buffer.
	 */
	Buffers(const Buffers& buf);

	/**
	 * @brief Move constructor. Transfers ownership from @p buf.
	 * @param buf Source buffer (left in the empty state, buf_ID = 0).
	 */
	Buffers(Buffers&& buf) noexcept;

	/**
	 * @brief Copy assignment. Performs a GPU-side deep copy of @p buf.
	 * @param buf Source buffer.
	 * @return Reference to this.
	 */
	Buffers& operator=(const Buffers& buf);

	/**
	 * @brief Move assignment. Transfers ownership from @p buf.
	 * @param buf Source buffer (left in the empty state).
	 * @return Reference to this.
	 */
	Buffers& operator=(Buffers&& buf) noexcept;

	/**
	 * @brief Returns the OpenGL buffer object ID.
	 * @return buf_ID (0 if the buffer is empty/invalid).
	 */
	GLuint GetID()   const { return buf_ID; }

	/**
	 * @brief Returns the allocated buffer size in bytes.
	 * @return buf_size (0 if no data has been uploaded).
	 */
	GLuint GetSize() const { return buf_size; }
};
