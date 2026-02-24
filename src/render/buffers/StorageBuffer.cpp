#include "StorageBuffer.h"

StorageBuffer::StorageBuffer(SSBType type)
	: StorageBuffer(type, 0)
{
}

StorageBuffer::StorageBuffer(SSBType type, GLuint base)
	: ssbo_base(base), ssbo_type(type)
{
	glGenBuffers(1, &buf_ID);
}

StorageBuffer::StorageBuffer(const StorageBuffer& ssbo)
	: GLBuffer(ssbo), ssbo_base(ssbo.ssbo_base), ssbo_type(ssbo.ssbo_type)
{
}

StorageBuffer::StorageBuffer(StorageBuffer&& ssbo) noexcept
	: GLBuffer(std::move(ssbo)), ssbo_base(ssbo.ssbo_base), ssbo_type(ssbo.ssbo_type)
{
}

StorageBuffer& StorageBuffer::operator=(const StorageBuffer& ssbo)
{
	if (this == &ssbo)
		return *this;

	GLBuffer::operator=(ssbo);
	ssbo_base = ssbo.ssbo_base;
	ssbo_type = ssbo.ssbo_type;
	return *this;
}

StorageBuffer& StorageBuffer::operator=(StorageBuffer&& ssbo) noexcept
{
	if (this == &ssbo)
		return *this;

	GLBuffer::operator=(std::move(ssbo));
	ssbo_base = ssbo.ssbo_base;
	ssbo_type = ssbo.ssbo_type;
	return *this;
}

void StorageBuffer::BindBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf_ID);
}

void StorageBuffer::UnbindBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void StorageBuffer::SetBufferBase(GLuint base)
{
	ssbo_base = base;
}

void StorageBuffer::BindBufferBase(GLuint _base) const
{
	if (_base == static_cast<GLuint>(-1)) _base = ssbo_base;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _base, buf_ID);
}
