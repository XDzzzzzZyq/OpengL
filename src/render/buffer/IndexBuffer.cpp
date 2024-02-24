#include "IndexBuffer.h"


void IndexBuffer::_delIBO()
{
	glDeleteBuffers(1, &ibo_id);
	ibo_id = 0;
}

IndexBuffer::IndexBuffer(const GLuint* data, GLuint size)

{
	//std::cout << getL(data) << std::endl;
	ibo_size = size;
	glGenBuffers(1, &ibo_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	//std::cout << size << std::endl;
}

IndexBuffer::IndexBuffer()
{
	ibo_id = 0;
	ibo_size = 0;
}

IndexBuffer::IndexBuffer(const IndexBuffer& ibo)
{
	_resetIBOID(ibo.GetID());
	ibo_size = ibo.ibo_size;
}

IndexBuffer::IndexBuffer(IndexBuffer&& ibo) noexcept
{
	_resetIBOID(ibo.GetID());
	ibo.ibo_id = 0;
	ibo_size = ibo.ibo_size;
}

IndexBuffer::~IndexBuffer()
{
	if (GetID() != 0)
		_delIBO();
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ibo)
{
	_resetIBOID(ibo.GetID());
	ibo_size = ibo.ibo_size;

	return *this;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ibo) noexcept
{
	_resetIBOID(ibo.GetID());
	ibo.ibo_id = 0;
	ibo_size = ibo.ibo_size;

	return *this;
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint IndexBuffer::Count() const
{
	return ibo_size / sizeof(GLuint);
}