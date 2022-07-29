#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>



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
void StorageBuffer::UpdateStorageBuffer(const std::vector<T>& src)
{
	BindBuffer();

	GLvoid* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, src.data(), src.size() * sizeof(T));

	UnbindBuffer();
}

template <typename T>
void StorageBuffer::GenStorageBuffer(const std::vector<T>& src)
{
	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, src.size() * sizeof(T), src.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_base, ssbo_id);
	UnbindBuffer();
}

