#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const GLuint* data, GLuint size)
{
	buf_size = size;
	glGenBuffers(1, &buf_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buf_size, data, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(const IndexBuffer& ibo) : GLBuffer(ibo) {}

IndexBuffer::IndexBuffer(IndexBuffer&& ibo) noexcept : GLBuffer(std::move(ibo)) {}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ibo)
{
	GLBuffer::operator=(ibo);
	return *this;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ibo) noexcept
{
	GLBuffer::operator=(std::move(ibo));
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