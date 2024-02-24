#pragma once
#include <GL/glew.h>
#include <iostream>
#include "structs.h"

class IndexBuffer
{
	void _delIBO();
	void _resetIBOID(GLuint _ID) { if (ibo_id > 0 && ibo_id != _ID)_delIBO(); ibo_id = _ID; }
public:

	IndexBuffer(const GLuint* data, GLuint size);
	IndexBuffer();
	~IndexBuffer();

	IndexBuffer(const IndexBuffer& ibo);
	IndexBuffer(IndexBuffer&& ibo) noexcept;

	IndexBuffer& operator=(const IndexBuffer& ibo);
	IndexBuffer& operator=(IndexBuffer&& ibo) noexcept;

public:

	void Bind() const;
	void Unbind() const;
	GLuint Count() const;
	GLuint GetID() const { return ibo_id; };

private:

	GLuint ibo_id;
	GLuint ibo_size;
};