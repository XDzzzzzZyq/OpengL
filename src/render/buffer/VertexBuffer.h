#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>

class VertexBuffer
{
private:
	GLuint vbo_id;
	GLuint vbo_size;
	void _delVBO();
	void _resetVBOID(GLuint _ID) { if (vbo_id > 0 && vbo_id != _ID)_delVBO(); vbo_id = _ID; }

public:

	VertexBuffer();
	VertexBuffer(float* data, GLuint size) = delete;
	VertexBuffer(const std::vector<float>& data);
	~VertexBuffer();

	VertexBuffer(const VertexBuffer& vbo);
	VertexBuffer(VertexBuffer&& vbo) noexcept;

	VertexBuffer& operator=(const VertexBuffer& vbo);
	VertexBuffer& operator=(VertexBuffer&& vbo) noexcept;

public:

	GLuint GetID() const;

	void Bind() const;
	void Unbind() const;
};

