#pragma once
#include <GL/glew.h>
#include <iostream>
#include "structs.h"

class IndexBuffer
{
public:
	IndexBuffer(const GLuint* data, GLuint size);
	IndexBuffer();
	~IndexBuffer();
	void DelIndBuff() const;
	void Bind() const;
	void Unbind() const;
	GLuint count() const;
private:
	GLuint m_ind_id;
	GLuint Bsize;
};