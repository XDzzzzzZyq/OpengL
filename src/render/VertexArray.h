#pragma once
#include "buffer/VertexBuffer.h"
#include "buffer/BufferLayout.h"

class VertexArray
{
private:
	GLuint m_renderID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const BufferLayout& bl); //add one buffer with its layout pattern

	void DelVertArr() const;

	void Bind() const;
	void Unbind() const;

	unsigned int GetVertArrayID() const {
		return m_renderID;
	}
};

