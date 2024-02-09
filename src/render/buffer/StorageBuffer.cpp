#include "StorageBuffer.h"

void StorageBuffer::_cpyInfo(const StorageBuffer& ssbo)
{
	ssbo_ID = ssbo.GetID(); ssbo_base = ssbo.ssbo_base; ssbo_type = ssbo.ssbo_type;
}

void StorageBuffer::_delSSB()
{
	glDeleteFramebuffers(1, &ssbo_ID);
	ssbo_ID = 0;
}

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
	glGenBuffers(1, &ssbo_ID);
}

StorageBuffer::~StorageBuffer()
{
	
}

void StorageBuffer::BindBuffer() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_ID);
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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_base, ssbo_ID);
}

StorageBuffer::StorageBuffer(const StorageBuffer& ssbo)
{
	_resetSSBID(ssbo.GetID());
	_cpyInfo(ssbo);
}

StorageBuffer::StorageBuffer(StorageBuffer&& ssbo) noexcept
{
	_cpyInfo(ssbo);
	ssbo.ssbo_ID = 0;
}

StorageBuffer& StorageBuffer::operator=(const StorageBuffer& ssbo)
{
	if (this == &ssbo)
		return *this;

	_resetSSBID(ssbo.GetID());
	_cpyInfo(ssbo);

	return *this;
}

StorageBuffer& StorageBuffer::operator=(StorageBuffer&& ssbo) noexcept
{
	if (this == &ssbo)
		return *this;

	_cpyInfo(ssbo);
	ssbo.ssbo_ID = 0;

	return *this;
}
