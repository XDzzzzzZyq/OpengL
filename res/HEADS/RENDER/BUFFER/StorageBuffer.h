#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>

#include "support.h"

enum SSBType
{
	NONE_LIST, FLOAT_LIST, INT_LIST, VEC2_LIST, VEC3_LIST
};

class StorageBuffer  //shader storage buffer object SSBO
{
private:
	GLuint ssbo_id;
	GLuint ssbo_base = 3;
	SSBType ssbo_type = NONE_LIST;

public:
	StorageBuffer();
	StorageBuffer(SSBType type);
	StorageBuffer(SSBType type, GLuint base);
	~StorageBuffer();

	void BindBuffer(GLuint _base = -1) const;
	void UnbindBuffer() const;
	void SetBufferBase(GLuint base);

	template <typename T>
	void GenStorageBuffer(const std::vector<T>& src);
};


template <typename T>
void StorageBuffer::GenStorageBuffer(const std::vector<T>& list)
{
	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_DYNAMIC_COPY);
	UnbindBuffer();
}

