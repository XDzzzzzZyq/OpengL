#pragma once

#include "buffer/VertexBuffer.h"
#include "buffer/BufferLayout.h"

class VertexArray
{
private:
	GLuint vao_id;
	GLuint vao_stride{};
	void _delVAO();
	void _resetVAOID(GLuint _ID) { if (vao_id > 0 && vao_id != _ID)_delVAO(); vao_id = _ID; }
public:

	VertexArray();
	~VertexArray();

	VertexArray(const VertexArray& vao);
	VertexArray(VertexArray&& vao) noexcept;

	VertexArray& operator=(const VertexArray& vao);
	VertexArray& operator=(VertexArray&& vao) noexcept;

	void AddBuffer(VertexBuffer& vb, BufferLayout bl); //add one buffer with its layout pattern

public:

	void Bind() const;
	void Unbind() const;

	unsigned int GetVertArrayID() const { return vao_id; }
	GLuint GetStride() const { return vao_stride / sizeof(GLfloat); };
};

