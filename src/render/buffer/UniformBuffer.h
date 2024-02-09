#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class UniformBuffer
{
	GLuint ubo_ID{ 0 };
	GLuint ubo_bind{ 0 };

	void _cpyInfo(const UniformBuffer& ubo);
	void _delUB();
	void _resetUBID(GLuint _ID) { if (ubo_ID > 0 && ubo_ID != _ID)_delUB(); ubo_ID = _ID; }

public:

	UniformBuffer() {};
	UniformBuffer(GLuint _bind);
	template<class _S>
	UniformBuffer(GLuint _bind, _S _tar);
	~UniformBuffer();

	UniformBuffer(const UniformBuffer& ubo);
	UniformBuffer(UniformBuffer&& ubo) noexcept;

	UniformBuffer& operator=(const UniformBuffer& ubo);
	UniformBuffer& operator=(UniformBuffer&& ubo) noexcept;

public:

	void Bind(GLuint _bind = -1) const;
	void Unbind() const;

public:

	template<class _S>
	void Update(_S _tar);

};


template<class _S>
void UniformBuffer::Update(_S _tar)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_ID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(_S), &_tar, GL_DYNAMIC_DRAW);
}

template<class _S>
UniformBuffer::UniformBuffer(GLuint _bind, _S _tar)
	:UniformBuffer(_bind)
{
	glBufferData(GL_UNIFORM_BUFFER, sizeof(_S), &_tar, GL_DYNAMIC_DRAW);
}
