/**
 * @file VertexBuffer.h
 * @brief OpenGL vertex buffer object (VBO) abstraction.
 * 
 * VertexBuffer provides RAII wrapper around OpenGL VBO for storing vertex
 * attribute data (positions, normals, UVs, etc.). Ensures deterministic
 * cleanup of GPU resources.
 * 
 * @note All OpenGL resources are released in destructor.
 */

#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>

/**
 * @brief OpenGL vertex buffer object wrapper.
 * 
 * VertexBuffer encapsulates a single VBO and manages its lifecycle. Supports
 * copy and move semantics for flexible resource management.
 * 
 * Usage:
 * @code
 * std::vector<float> vertices = { ... };
 * VertexBuffer vbo(vertices);
 * vbo.Bind();
 * // Configure vertex attributes...
 * vbo.Unbind();
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 * @note Ownership: Owns OpenGL VBO and releases it in destructor.
 * @note TODO: Fix copy semantics - current implementation duplicates VBO ID without deep copy or ref counting
 */
class VertexBuffer
{
private:
	GLuint vbo_id;   ///< OpenGL vertex buffer object ID
	GLuint vbo_size; ///< Buffer size in bytes
	
	void _delVBO(); ///< Deletes OpenGL buffer object
	
	/**
	 * @brief Resets VBO ID, deleting old buffer if different.
	 * @param _ID New VBO ID
	 */
	void _resetVBOID(GLuint _ID) { if (vbo_id > 0 && vbo_id != _ID)_delVBO(); vbo_id = _ID; }

public:

	/**
	 * @brief Default constructor (empty buffer).
	 */
	VertexBuffer();
	
	/**
	 * @brief Constructs VBO from raw float array (deleted to prevent dangling pointers).
	 * @param data Pointer to float array
	 * @param size Array size in floats
	 */
	VertexBuffer(float* data, GLuint size) = delete;
	
	/**
	 * @brief Constructs VBO from vector of floats.
	 * @param data Vertex data (interleaved attributes)
	 * @note Uploads data to GPU using GL_STATIC_DRAW
	 */
	VertexBuffer(const std::vector<float>& data);
	
	/**
	 * @brief Destructor. Releases OpenGL VBO.
	 */
	~VertexBuffer();

	/**
	 * @brief Copy constructor.
	 * @param vbo Source vertex buffer
	 * @note TODO: Current implementation duplicates VBO ID - fix to deep copy or delete
	 */
	VertexBuffer(const VertexBuffer& vbo);
	
	/**
	 * @brief Move constructor.
	 * @param vbo Source vertex buffer (invalidated)
	 */
	VertexBuffer(VertexBuffer&& vbo) noexcept;

	/**
	 * @brief Copy assignment operator.
	 * @param vbo Source vertex buffer
	 * @return Reference to this
	 */
	VertexBuffer& operator=(const VertexBuffer& vbo);
	
	/**
	 * @brief Move assignment operator.
	 * @param vbo Source vertex buffer (invalidated)
	 * @return Reference to this
	 */
	VertexBuffer& operator=(VertexBuffer&& vbo) noexcept;

public:

	/**
	 * @brief Returns OpenGL buffer object ID.
	 * @return VBO ID
	 */
	GLuint GetID() const;

	/**
	 * @brief Binds this VBO for use (GL_ARRAY_BUFFER target).
	 * @note Subsequent glVertexAttribPointer calls configure this buffer
	 */
	void Bind() const;
	
	/**
	 * @brief Unbinds the current VBO (binds VBO 0).
	 */
	void Unbind() const;
};

