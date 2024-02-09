#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(GLuint _bind)
	:ubo_bind(_bind)
{
	glGenBuffers(1, &ubo_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_ID);
}

UniformBuffer::~UniformBuffer()
{
	if (ubo_ID != 0)
		_delUB();
}

void UniformBuffer::_cpyInfo(const UniformBuffer& ubo)
{
	ubo_ID = ubo.ubo_ID; ubo_bind = ubo.ubo_bind;
}


void UniformBuffer::_delUB()
{
	glDeleteFramebuffers(1, &ubo_ID);
	ubo_ID = 0;
}


UniformBuffer::UniformBuffer(const UniformBuffer& ubo)
{
	_resetUBID(ubo.ubo_ID);
	_cpyInfo(ubo);
}


UniformBuffer::UniformBuffer(UniformBuffer&& ubo) noexcept
{
	_cpyInfo(ubo);
	ubo.ubo_ID = 0;
}


UniformBuffer& UniformBuffer::operator=(const UniformBuffer& ubo)
{
	if (this == &ubo)
		return *this;

	_resetUBID(ubo.ubo_ID);
	_cpyInfo(ubo);

	return *this;
}


UniformBuffer& UniformBuffer::operator=(UniformBuffer&& ubo) noexcept
{
	if (this == &ubo)
		return *this;

	_cpyInfo(ubo);
	ubo.ubo_ID = 0;

	return *this;
}

void UniformBuffer::Bind(GLuint _bind /*= -1*/) const
{
	if (_bind == -1) _bind = ubo_bind;

	glBindBufferBase(GL_UNIFORM_BUFFER, _bind, ubo_ID);
}

void UniformBuffer::Unbind() const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_bind, 0);
}
