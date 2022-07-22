#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(float* data, GLuint size)
{
	Bufferdata = data;
	Buffersize = size;
	glGenBuffers(1, &m_ver_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_ver_id);

	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer()
{
	Bufferdata = nullptr;
	Buffersize = 0;
	m_ver_id = 0;
}

GLuint VertexBuffer::GetID()
{
	return m_ver_id;
}

float* VertexBuffer::GetData()
{
	return Bufferdata;
}

VertexBuffer::~VertexBuffer()
{

	//if(Bufferdata)delete Bufferdata;
}

void VertexBuffer::DelVertBuff() const
{
	glDeleteBuffers(1, &m_ver_id);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_ver_id);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
