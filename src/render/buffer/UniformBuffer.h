#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

template<class _S>
class UniformBuffer
{
	static constexpr size_t _Size = sizeof(_S);
	GLuint ubo_id{ 0 };
	GLuint ubo_bind{ 0 };

public:

	UniformBuffer();
	UniformBuffer(GLuint _bind);
	UniformBuffer(GLuint _bind, _S _tar);

public:

	void Bind(GLuint _bind = -1) const;
	void Unbind() const;

public:

	void Update(_S _tar);

public:

	void DeleteBuffer() const;

};

template<class _S>
void UniformBuffer<_S>::Update(_S _tar)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_id);
	glBufferData(GL_UNIFORM_BUFFER, _Size, &_tar, GL_DYNAMIC_DRAW);
}

template<class _S>
void UniformBuffer<_S>::Unbind() const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_bind, 0);
}

template<class _S>
void UniformBuffer<_S>::Bind(GLuint _bind /*= -1*/) const
{
	if (_bind == -1) _bind = ubo_bind;

	glBindBufferBase(GL_UNIFORM_BUFFER, _bind, ubo_id);
}

template<class _S>
UniformBuffer<_S>::UniformBuffer(GLuint _bind, _S _tar)
	:ubo_bind(_bind)
{
	glGenBuffers(1, &ubo_id);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_id);

	glBufferData(GL_UNIFORM_BUFFER, _Size, &_tar, GL_DYNAMIC_DRAW);
}

template<class _S>
UniformBuffer<_S>::UniformBuffer(GLuint _bind)
	:UniformBuffer(_bind, _S{})
{}

template<class _S>
UniformBuffer<_S>::UniformBuffer()
{}

template<class _S>
void UniformBuffer<_S>::DeleteBuffer() const
{
	glDeleteBuffers(1, &ubo_id);
}

