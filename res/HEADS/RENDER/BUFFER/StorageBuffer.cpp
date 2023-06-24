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
	glGenBuffers(1, &ssbo_id);
}

StorageBuffer::~StorageBuffer()
{
	
}

void StorageBuffer::BindBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
}

void StorageBuffer::UnbindBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void StorageBuffer::SetBufferBase(GLuint base)
{
	ssbo_base = base;
}

void StorageBuffer::BindBufferBase(GLuint _base /*= -1*/) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_base, ssbo_id);
}

void StorageBuffer::DeleteBuffer()
{
	glDeleteBuffers(1, &ssbo_id);
}

