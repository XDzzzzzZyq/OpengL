#include "StorageBuffer.h"

StorageBuffer::StorageBuffer()
{
}

StorageBuffer::StorageBuffer(SSBType type)
	: StorageBuffer(type, 0)
{
}

StorageBuffer::StorageBuffer(SSBType type, GLuint base)
	: ssbo_base(base), ssbo_type(type)
{
	//glGenBuffers(1, &ssbo_id);
}

StorageBuffer::~StorageBuffer()
{
	glDeleteBuffers(1, &ssbo_id);
}

void StorageBuffer::BindBuffer(GLuint _base) const
{
	if (_base == -1)
		_base = ssbo_base;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _base, ssbo_id);
}

void StorageBuffer::UnbindBuffer() const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0    , ssbo_id);
}

void StorageBuffer::SetBufferBase(GLuint base)
{
	ssbo_base = base;
}

