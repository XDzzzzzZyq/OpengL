/**
 * @file UniformBuffer.h
 * @brief OpenGL uniform buffer object (UBO) abstraction.
 *
 * UniformBuffer provides a RAII wrapper around an OpenGL UBO for efficient
 * sharing of uniform data across multiple shaders. Inherits copy, move, and
 * deep-copy semantics from Buffers. Supports template-based updates for
 * type-safe data transfer.
 *
 * @note All OpenGL resources are released deterministically in the destructor.
 */

#pragma once

#include "Buffers.h"
#include <GLFW/glfw3.h>

/**
 * @brief OpenGL uniform buffer object wrapper with template-based updates.
 *
 * UniformBuffer encapsulates a UBO for sharing uniform blocks across shaders.
 * It inherits RAII resource management from Buffers, including GPU-side deep
 * copy via glCopyBufferSubData. buf_size is updated whenever Update() or the
 * initialising constructor is called.
 *
 * More efficient than individual uniforms when sharing large data structures
 * (e.g., camera matrices, lighting data) across multiple shader programs.
 *
 * Usage:
 * @code
 * struct CameraData {
 *     glm::mat4 view;
 *     glm::mat4 projection;
 * };
 *
 * UniformBuffer ubo(0, CameraData{viewMat, projMat});
 * ubo.Bind();
 * // Update on camera change
 * ubo.Update(CameraData{newView, newProj});
 *
 * // Deep copy (new GPU buffer allocated):
 * UniformBuffer copy = ubo;
 * @endcode
 *
 * @note Thread-safety: Not thread-safe. Must be used from the OpenGL context thread.
 * @note Ownership: Inherits Buffers ownership; releases via glDeleteBuffers in destructor.
 * @note Binding point: UBOs use binding points to link with shader uniform blocks.
 */
class UniformBuffer : public Buffers
{
	GLuint ubo_bind{ 0 }; ///< Binding point index

public:

	/**
	 * @brief Default constructor (empty buffer).
	 */
	UniformBuffer() = default;

	/**
	 * @brief Constructs UBO with a binding point (no initial data).
	 * @param _bind Binding point index (must match shader layout).
	 */
	UniformBuffer(GLuint _bind);

	/**
	 * @brief Constructs UBO with binding point and initial data.
	 * @tparam _S Data structure type
	 * @param _bind Binding point index
	 * @param _tar  Initial data to upload
	 * @note Uses GL_DYNAMIC_DRAW. Sets buf_size = sizeof(_S).
	 */
	template<class _S>
	UniformBuffer(GLuint _bind, _S _tar);

	/**
	 * @brief Destructor. Releases OpenGL UBO (via Buffers).
	 */
	~UniformBuffer() = default;

	/**
	 * @brief Copy constructor. Deep-copies GPU buffer content and binding point.
	 * @param ubo Source uniform buffer.
	 */
	UniformBuffer(const UniformBuffer& ubo);

	/**
	 * @brief Move constructor. Transfers ownership; @p ubo is left empty.
	 * @param ubo Source uniform buffer (invalidated after move).
	 */
	UniformBuffer(UniformBuffer&& ubo) noexcept;

	/**
	 * @brief Copy assignment. Deep-copies GPU buffer content and binding point.
	 * @param ubo Source uniform buffer.
	 * @return Reference to this.
	 */
	UniformBuffer& operator=(const UniformBuffer& ubo);

	/**
	 * @brief Move assignment. Transfers ownership; @p ubo is left empty.
	 * @param ubo Source uniform buffer (invalidated after move).
	 * @return Reference to this.
	 */
	UniformBuffer& operator=(UniformBuffer&& ubo) noexcept;

public:

	/**
	 * @brief Binds UBO to its binding point.
	 * @param _bind Binding point override (-1 to use stored binding).
	 */
	void Bind(GLuint _bind = static_cast<GLuint>(-1)) const;

	/**
	 * @brief Unbinds the current UBO from its binding point.
	 */
	void Unbind() const;

public:

	/**
	 * @brief Uploads new data to the UBO and records the buffer size.
	 * @tparam _S Data structure type (must match UBO layout)
	 * @param _tar New data to upload
	 * @note Uses GL_DYNAMIC_DRAW for efficient updates. Sets buf_size = sizeof(_S).
	 */
	template<class _S>
	void Update(_S _tar);

};


template<class _S>
void UniformBuffer::Update(_S _tar)
{
	buf_size = static_cast<GLuint>(sizeof(_S));
	glBindBuffer(GL_UNIFORM_BUFFER, buf_ID);
	glBufferData(GL_UNIFORM_BUFFER, buf_size, &_tar, GL_DYNAMIC_DRAW);
}

template<class _S>
UniformBuffer::UniformBuffer(GLuint _bind, _S _tar)
	: UniformBuffer(_bind)
{
	buf_size = static_cast<GLuint>(sizeof(_S));
	glBufferData(GL_UNIFORM_BUFFER, buf_size, &_tar, GL_DYNAMIC_DRAW);
}
