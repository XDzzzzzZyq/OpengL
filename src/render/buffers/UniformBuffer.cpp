#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(GLuint _bind)
	: ubo_bind(_bind)
{
	glGenBuffers(1, &buf_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, buf_ID);
}

UniformBuffer::UniformBuffer(const UniformBuffer& ubo)
	: GLBuffer(ubo), ubo_bind(ubo.ubo_bind)
{
}

UniformBuffer::UniformBuffer(UniformBuffer&& ubo) noexcept
	: GLBuffer(std::move(ubo)), ubo_bind(ubo.ubo_bind)
{
}

UniformBuffer& UniformBuffer::operator=(const UniformBuffer& ubo)
{
	if (this == &ubo)
		return *this;

	GLBuffer::operator=(ubo);
	ubo_bind = ubo.ubo_bind;
	return *this;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& ubo) noexcept
{
	if (this == &ubo)
		return *this;

	GLBuffer::operator=(std::move(ubo));
	ubo_bind = ubo.ubo_bind;
	return *this;
}

void UniformBuffer::Bind(GLuint _bind) const
{
	if (_bind == static_cast<GLuint>(-1)) _bind = ubo_bind;
	glBindBufferBase(GL_UNIFORM_BUFFER, _bind, buf_ID);
}

void UniformBuffer::Unbind() const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_bind, 0);
}
