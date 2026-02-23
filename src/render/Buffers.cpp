#include "Buffers.h"

void GLBuffer::_delBuffer()
{
	glDeleteBuffers(1, &buf_ID);
	buf_ID = 0;
}

void GLBuffer::_deepCopyFrom(const GLBuffer& src)
{
	buf_size = src.buf_size;

	if (src.buf_ID == 0)
	{
		buf_ID = 0;
		return;
	}

	glGenBuffers(1, &buf_ID);

	// Nothing to copy for a zero-size allocation (e.g. SSBO created but not yet filled).
	if (buf_size == 0)
		return;

	glBindBuffer(GL_COPY_READ_BUFFER,  src.buf_ID);
	glBindBuffer(GL_COPY_WRITE_BUFFER, buf_ID);
	glBufferData(GL_COPY_WRITE_BUFFER, buf_size, nullptr, GL_STATIC_DRAW);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, buf_size);
	glBindBuffer(GL_COPY_READ_BUFFER,  0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

GLBuffer::~GLBuffer()
{
	if (buf_ID != 0)
		_delBuffer();
}

GLBuffer::GLBuffer(const GLBuffer& buf)
{
	_deepCopyFrom(buf);
}

GLBuffer::GLBuffer(GLBuffer&& buf) noexcept
{
	buf_ID   = buf.buf_ID;
	buf_size = buf.buf_size;
	buf.buf_ID = 0;
}

GLBuffer& GLBuffer::operator=(const GLBuffer& buf)
{
	if (this == &buf)
		return *this;

	if (buf_ID != 0)
		_delBuffer();

	_deepCopyFrom(buf);
	return *this;
}

GLBuffer& GLBuffer::operator=(GLBuffer&& buf) noexcept
{
	if (this == &buf)
		return *this;

	if (buf_ID != 0)
		_delBuffer();

	buf_ID   = buf.buf_ID;
	buf_size = buf.buf_size;
	buf.buf_ID = 0;
	return *this;
}
