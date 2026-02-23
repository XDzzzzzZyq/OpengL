/**
 * @file IndexBuffer.h
 * @brief OpenGL index buffer object (IBO/EBO) abstraction.
 *
 * IndexBuffer provides a RAII wrapper around an OpenGL element buffer for
 * storing triangle indices. Inherits copy, move, and deep-copy semantics
 * from GLBuffer.
 *
 * @note All OpenGL resources are released deterministically in the destructor.
 */

#pragma once

#include "Buffers.h"

/**
 * @brief OpenGL element buffer object wrapper.
 *
 * IndexBuffer encapsulates a single IBO/EBO bound to GL_ELEMENT_ARRAY_BUFFER
 * for indexed rendering. It inherits RAII resource management from GLBuffer,
 * including GPU-side deep copy via glCopyBufferSubData.
 *
 * Usage:
 * @code
 * GLuint indices[] = {0, 1, 2, 2, 3, 0};
 * IndexBuffer ibo(indices, sizeof(indices));
 * ibo.Bind();
 * glDrawElements(GL_TRIANGLES, ibo.Count(), GL_UNSIGNED_INT, 0);
 *
 * // Deep copy (new GPU buffer allocated):
 * IndexBuffer copy = ibo;
 *
 * // Transfer ownership (ibo is empty after this):
 * IndexBuffer moved = std::move(ibo);
 * @endcode
 *
 * @note Thread-safety: Not thread-safe. Must be used from the OpenGL context thread.
 * @note Ownership: Inherits GLBuffer ownership; releases via glDeleteBuffers in destructor.
 * @note @p size parameter in the main constructor is in bytes, not element count.
 */
class IndexBuffer : public GLBuffer
{
public:

	/**
	 * @brief Constructs IBO from an index array.
	 * @param data Pointer to index data (GLuint array)
	 * @param size Size in bytes (= element count * sizeof(GLuint))
	 * @note Uploads data to GPU using GL_STATIC_DRAW. Sets buf_size accordingly.
	 */
	IndexBuffer(const GLuint* data, GLuint size);

	/**
	 * @brief Default constructor (empty buffer, buf_ID = 0).
	 */
	IndexBuffer() = default;

	/**
	 * @brief Destructor. Releases OpenGL IBO (via GLBuffer).
	 */
	~IndexBuffer() = default;

	/**
	 * @brief Copy constructor. Deep-copies GPU buffer content.
	 * @param ibo Source index buffer.
	 */
	IndexBuffer(const IndexBuffer& ibo);

	/**
	 * @brief Move constructor. Transfers ownership; @p ibo is left empty.
	 * @param ibo Source index buffer (invalidated after move).
	 */
	IndexBuffer(IndexBuffer&& ibo) noexcept;

	/**
	 * @brief Copy assignment. Deep-copies GPU buffer content.
	 * @param ibo Source index buffer.
	 * @return Reference to this.
	 */
	IndexBuffer& operator=(const IndexBuffer& ibo);

	/**
	 * @brief Move assignment. Transfers ownership; @p ibo is left empty.
	 * @param ibo Source index buffer (invalidated after move).
	 * @return Reference to this.
	 */
	IndexBuffer& operator=(IndexBuffer&& ibo) noexcept;

public:

	/**
	 * @brief Binds this IBO to GL_ELEMENT_ARRAY_BUFFER.
	 */
	void Bind() const;

	/**
	 * @brief Unbinds the current IBO (binds GL_ELEMENT_ARRAY_BUFFER to 0).
	 */
	void Unbind() const;

	/**
	 * @brief Returns the number of indices.
	 * @return Index count (buf_size / sizeof(GLuint)).
	 * @note Pass as the @p count argument to glDrawElements.
	 */
	GLuint Count() const;

	/**
	 * @brief Returns the OpenGL buffer object ID (inherited from GLBuffer).
	 * @return buf_ID.
	 */
	GLuint GetID() const { return buf_ID; }
};