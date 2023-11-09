#include "IndexBuffer.h"


IndexBuffer::IndexBuffer(const GLuint* data, GLuint size)

{
	//std::cout << getL(data) << std::endl;
	Bsize = size;
	glGenBuffers(1, &m_ind_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ind_id);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	//std::cout << size << std::endl;
}

IndexBuffer::IndexBuffer()
{
	m_ind_id = 0;
	Bsize = 0;
}

IndexBuffer::~IndexBuffer()
{

}

void IndexBuffer::DelIndBuff() const
{
	glDeleteBuffers(1, &m_ind_id);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ind_id);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint IndexBuffer::count() const {
	return Bsize / sizeof(GLuint);
}