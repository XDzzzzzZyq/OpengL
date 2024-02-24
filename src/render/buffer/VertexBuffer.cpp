#include "VertexBuffer.h"

void VertexBuffer::_delVBO()
{
	glDeleteBuffers(1, &vbo_id);
	vbo_id = 0;
}

/*
VertexBuffer::VertexBuffer(float* data, GLuint size)
{
	vbo_size = size;
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
*/
VertexBuffer::VertexBuffer(const std::vector<float>& data)
{
	vbo_size = data.size() * sizeof(float);
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

	glBufferData(GL_ARRAY_BUFFER, vbo_size, data.data(), GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer()
{}

VertexBuffer::VertexBuffer(const VertexBuffer& vbo)
{
	_resetVBOID(vbo.GetID());
	vbo_size = vbo.vbo_size;
}

VertexBuffer::VertexBuffer(VertexBuffer&& vbo) noexcept
{
	_resetVBOID(vbo.GetID());
	vbo.vbo_id = 0;
	vbo_size = vbo.vbo_size;
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vbo)
{
	_resetVBOID(vbo.GetID());
	vbo_size = vbo.vbo_size;

	return *this;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vbo) noexcept
{
	_resetVBOID(vbo.GetID());
	vbo_size = vbo.vbo_size;
	vbo.vbo_id = 0;

	return *this;
}

GLuint VertexBuffer::GetID() const
{
	return vbo_id;
}

VertexBuffer::~VertexBuffer()
{
	if (GetID() != 0)
		_delVBO();
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
