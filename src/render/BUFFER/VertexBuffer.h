#pragma once

#include <GL/glew.h>
#include <iostream>

class VertexBuffer
{
public:
	GLuint m_ver_id;
	float* Bufferdata;
	GLuint Buffersize;

	VertexBuffer();
	VertexBuffer(float* data, GLuint size);
	GLuint GetID();
	float* GetData();
	~VertexBuffer();
	void DelVertBuff() const;
	void Bind() const;
	void Unbind() const;
private:
};

