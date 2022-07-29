#include "StorageBuffer.h"

StorageBuffer::StorageBuffer()
{
	glGenBuffers(1, &ssbo_id);
}

StorageBuffer::~StorageBuffer()
{
	glDeleteBuffers(1, &ssbo_id);
}

void StorageBuffer::BindBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
}

void StorageBuffer::UnbindBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void StorageBuffer::SetBufferBase(int base)
{

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, base, ssbo_id);
	ssbo_base = base;
}

