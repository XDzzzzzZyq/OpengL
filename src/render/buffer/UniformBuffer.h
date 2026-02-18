/**
 * @file UniformBuffer.h
 * @brief OpenGL uniform buffer object (UBO) abstraction.
 * 
 * UniformBuffer provides RAII wrapper around OpenGL UBO for efficient sharing
 * of uniform data across multiple shaders. Supports template-based updates
 * for type-safe data transfer.
 * 
 * @note All OpenGL resources are released in destructor.
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
 * @brief OpenGL uniform buffer object wrapper with template-based updates.
 * 
 * UniformBuffer encapsulates a UBO for sharing uniform blocks across shaders.
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
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 * @note Ownership: Owns OpenGL UBO and releases it in destructor.
 * @note Binding point: UBOs use binding points to link with shader uniform blocks.
 */
class UniformBuffer
{
	GLuint ubo_ID{ 0 };   ///< OpenGL uniform buffer object ID
	GLuint ubo_bind{ 0 }; ///< Binding point index
	
	void _cpyInfo(const UniformBuffer& ubo); ///< Copies UBO info
	void _delUB();                            ///< Deletes OpenGL uniform buffer
	
	/**
	 * @brief Resets UBO ID, deleting old buffer if different.
	 * @param _ID New UBO ID
	 */
	void _resetUBID(GLuint _ID) { if (ubo_ID > 0 && ubo_ID != _ID)_delUB(); ubo_ID = _ID; }

public:

	/**
	 * @brief Default constructor (empty buffer).
	 */
	UniformBuffer() {};
	
	/**
	 * @brief Constructs UBO with binding point.
	 * @param _bind Binding point index (must match shader layout)
	 */
	UniformBuffer(GLuint _bind);
	
	/**
	 * @brief Constructs UBO with initial data.
	 * @tparam _S Data structure type
	 * @param _bind Binding point index
	 * @param _tar Initial data to upload
	 * @note Uses GL_DYNAMIC_DRAW for frequent updates
	 */
	template<class _S>
	UniformBuffer(GLuint _bind, _S _tar);
	
	/**
	 * @brief Destructor. Releases OpenGL UBO.
	 */
	~UniformBuffer();

	/**
	 * @brief Copy constructor.
	 * @param ubo Source uniform buffer
	 */
	UniformBuffer(const UniformBuffer& ubo);
	
	/**
	 * @brief Move constructor.
	 * @param ubo Source uniform buffer (invalidated)
	 */
	UniformBuffer(UniformBuffer&& ubo) noexcept;

	/**
	 * @brief Copy assignment operator.
	 * @param ubo Source uniform buffer
	 * @return Reference to this
	 */
	UniformBuffer& operator=(const UniformBuffer& ubo);
	
	/**
	 * @brief Move assignment operator.
	 * @param ubo Source uniform buffer (invalidated)
	 * @return Reference to this
	 */
	UniformBuffer& operator=(UniformBuffer&& ubo) noexcept;

public:

	/**
	 * @brief Binds UBO to its binding point.
	 * @param _bind Binding point override (default: use stored binding)
	 */
	void Bind(GLuint _bind = -1) const;
	
	/**
	 * @brief Unbinds the current UBO.
	 */
	void Unbind() const;

public:

	/**
	 * @brief Updates UBO data.
	 * @tparam _S Data structure type (must match UBO layout)
	 * @param _tar New data to upload
	 * @note Uses GL_DYNAMIC_DRAW for efficient updates
	 */
	template<class _S>
	void Update(_S _tar);

};


template<class _S>
void UniformBuffer::Update(_S _tar)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_ID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(_S), &_tar, GL_DYNAMIC_DRAW);
}

template<class _S>
UniformBuffer::UniformBuffer(GLuint _bind, _S _tar)
	:UniformBuffer(_bind)
{
	glBufferData(GL_UNIFORM_BUFFER, sizeof(_S), &_tar, GL_DYNAMIC_DRAW);
}
