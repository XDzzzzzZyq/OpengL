/**
 * @file VertexBuffer.h
 * @brief OpenGL vertex buffer object (VBO) abstraction.
 *
 * VertexBuffer provides a RAII wrapper around an OpenGL VBO for storing
 * vertex attribute data (positions, normals, UVs, etc.). Inherits copy,
 * move, and deep-copy semantics from Buffers.
 *
 * @note All OpenGL resources are released deterministically in the destructor.
 */

#pragma once

#include "Buffers.h"
#include <vector>

/**
 * @brief OpenGL vertex buffer object wrapper.
 *
 * VertexBuffer encapsulates a single VBO bound to GL_ARRAY_BUFFER. It
 * inherits RAII resource management from Buffers, including GPU-side deep
 * copy via glCopyBufferSubData.
 *
 * Usage:
 * @code
 * std::vector<float> vertices = { ... };
 * VertexBuffer vbo(vertices);
 * vbo.Bind();
 * // Configure vertex attributes...
 * vbo.Unbind();
 *
 * // Deep copy (new GPU buffer allocated):
 * VertexBuffer copy = vbo;
 *
 * // Transfer ownership (vbo is empty after this):
 * VertexBuffer moved = std::move(vbo);
 * @endcode
 *
 * @note Thread-safety: Not thread-safe. Must be used from the OpenGL context thread.
 * @note Ownership: Inherits Buffers ownership; releases via glDeleteBuffers in destructor.
 */
class VertexBuffer : public Buffers
{
public:

	/**
	 * @brief Default constructor (empty buffer, buf_ID = 0).
	 */
	VertexBuffer() = default;

	/**
	 * @brief Constructs VBO from raw float array (deleted to prevent dangling pointers).
	 * @param data Pointer to float array
	 * @param size Array size in floats
	 */
	VertexBuffer(float* data, GLuint size) = delete;

	/**
	 * @brief Constructs VBO from a vector of floats.
	 * @param data Vertex data (interleaved attributes)
	 * @note Uploads data to GPU using GL_STATIC_DRAW. Sets buf_size accordingly.
	 */
	VertexBuffer(const std::vector<float>& data);

	/**
	 * @brief Destructor. Releases OpenGL VBO (via Buffers).
	 */
	~VertexBuffer() = default;

	/**
	 * @brief Copy constructor. Deep-copies GPU buffer content.
	 * @param vbo Source vertex buffer.
	 */
	VertexBuffer(const VertexBuffer& vbo);

	/**
	 * @brief Move constructor. Transfers ownership; @p vbo is left empty.
	 * @param vbo Source vertex buffer (invalidated after move).
	 */
	VertexBuffer(VertexBuffer&& vbo) noexcept;

	/**
	 * @brief Copy assignment. Deep-copies GPU buffer content.
	 * @param vbo Source vertex buffer.
	 * @return Reference to this.
	 */
	VertexBuffer& operator=(const VertexBuffer& vbo);

	/**
	 * @brief Move assignment. Transfers ownership; @p vbo is left empty.
	 * @param vbo Source vertex buffer (invalidated after move).
	 * @return Reference to this.
	 */
	VertexBuffer& operator=(VertexBuffer&& vbo) noexcept;

public:

	/**
	 * @brief Binds this VBO to GL_ARRAY_BUFFER.
	 * @note Subsequent glVertexAttribPointer calls configure this buffer.
	 */
	void Bind() const;

	/**
	 * @brief Unbinds the current VBO (binds GL_ARRAY_BUFFER to 0).
	 */
	void Unbind() const;
};

