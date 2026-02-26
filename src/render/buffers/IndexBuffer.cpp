#include "IndexBuffer.h"
#include <utility>

IndexBuffer::IndexBuffer(const GLuint* data, GLuint size)
{
	buf_size = size;
	glGenBuffers(1, &buf_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buf_size, data, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(const IndexBuffer& ibo) : Buffers(ibo) {}

IndexBuffer::IndexBuffer(IndexBuffer&& ibo) noexcept : Buffers(std::move(ibo)) {}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ibo)
{
	Buffers::operator=(ibo);
	return *this;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ibo) noexcept
{
	Buffers::operator=(std::move(ibo));
	return *this;
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ID);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint IndexBuffer::Count() const
{
	return buf_size / sizeof(GLuint);
}