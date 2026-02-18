/**
 * @file IndexBuffer.h
 * @brief OpenGL index buffer object (IBO/EBO) abstraction.
 * 
 * IndexBuffer provides RAII wrapper around OpenGL element buffer for storing
 * triangle indices. Used with VertexBuffer for indexed geometry rendering.
 * 
 * @note All OpenGL resources are released in destructor.
 */

#pragma once
#include <GL/glew.h>
#include <iostream>
#include "structs.h"

/**
 * @brief OpenGL element buffer object wrapper.
 * 
 * IndexBuffer encapsulates a single IBO/EBO for indexed rendering. Stores
 * triangle indices (typically uint32) to reference vertices in VBO.
 * 
 * Usage:
 * @code
 * GLuint indices[] = {0, 1, 2, 2, 3, 0};
 * IndexBuffer ibo(indices, 6);
 * ibo.Bind();
 * glDrawElements(GL_TRIANGLES, ibo.Count(), GL_UNSIGNED_INT, 0);
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 * @note Ownership: Owns OpenGL IBO and releases it in destructor.
 */
class IndexBuffer
{
	void _delIBO();  ///< Deletes OpenGL index buffer object
	
	/**
	 * @brief Resets IBO ID, deleting old buffer if different.
	 * @param _ID New IBO ID
	 */
	void _resetIBOID(GLuint _ID) { if (ibo_id > 0 && ibo_id != _ID)_delIBO(); ibo_id = _ID; }
	
public:

	/**
	 * @brief Constructs IBO from index array.
	 * @param data Pointer to index data (GLuint array)
	 * @param size Number of indices
	 * @note Uploads data to GPU using GL_STATIC_DRAW
	 */
	IndexBuffer(const GLuint* data, GLuint size);
	
	/**
	 * @brief Default constructor (empty buffer).
	 */
	IndexBuffer();
	
	/**
	 * @brief Destructor. Releases OpenGL IBO.
	 */
	~IndexBuffer();

	/**
	 * @brief Copy constructor.
	 * @param ibo Source index buffer
	 */
	IndexBuffer(const IndexBuffer& ibo);
	
	/**
	 * @brief Move constructor.
	 * @param ibo Source index buffer (invalidated)
	 */
	IndexBuffer(IndexBuffer&& ibo) noexcept;

	/**
	 * @brief Copy assignment operator.
	 * @param ibo Source index buffer
	 * @return Reference to this
	 */
	IndexBuffer& operator=(const IndexBuffer& ibo);
	
	/**
	 * @brief Move assignment operator.
	 * @param ibo Source index buffer (invalidated)
	 * @return Reference to this
	 */
	IndexBuffer& operator=(IndexBuffer&& ibo) noexcept;

public:

	/**
	 * @brief Binds this IBO for use (GL_ELEMENT_ARRAY_BUFFER target).
	 */
	void Bind() const;
	
	/**
	 * @brief Unbinds the current IBO (binds IBO 0).
	 */
	void Unbind() const;
	
	/**
	 * @brief Returns number of indices.
	 * @return Index count
	 * @note Used as count parameter in glDrawElements
	 */
	GLuint Count() const;
	
	/**
	 * @brief Returns OpenGL buffer object ID.
	 * @return IBO ID
	 */
	GLuint GetID() const { return ibo_id; };

private:

	GLuint ibo_id;   ///< OpenGL index buffer object ID
	GLuint ibo_size; ///< Number of indices
};