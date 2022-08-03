#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>

#include "support.h"

class StorageBuffer  //shader storage buffer object SSBO
{
private:
	GLuint ssbo_id = 0;
	GLuint ssbo_base = 3;
public:
	StorageBuffer();
	~StorageBuffer();

	void BindBuffer() const;
	void UnbindBuffer() const;
	void SetBufferBase(int base);

	template <typename T>
	void GenStorageBuffer(const std::vector<T>& src);

	template <typename T>
	void UpdateStorageBuffer(const std::vector<T>& src);
};

template <typename T>
void StorageBuffer::UpdateStorageBuffer(const std::vector<T>& list)
{
	BindBuffer();
	//GLDEBUG
	GLvoid* ptr = glMapNamedBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//GLDEBUG
	memcpy(ptr, list.data(), list.size() * sizeof(T));

	UnbindBuffer();
}

template <typename T>
void StorageBuffer::GenStorageBuffer(const std::vector<T>& list)
{

	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_base, ssbo_id);
	//DEBUG( list.size() * sizeof(T))
	UnbindBuffer();
}

