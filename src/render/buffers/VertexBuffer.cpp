#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const std::vector<float>& data)
{
	buf_size = static_cast<GLuint>(data.size() * sizeof(float));
	glGenBuffers(1, &buf_ID);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ID);
	glBufferData(GL_ARRAY_BUFFER, buf_size, data.data(), GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(const VertexBuffer& vbo) : Buffers(vbo) {}

VertexBuffer::VertexBuffer(VertexBuffer&& vbo) noexcept : Buffers(std::move(vbo)) {}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vbo)
{
	Buffers::operator=(vbo);
	return *this;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vbo) noexcept
{
	Buffers::operator=(std::move(vbo));
	return *this;
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, buf_ID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
