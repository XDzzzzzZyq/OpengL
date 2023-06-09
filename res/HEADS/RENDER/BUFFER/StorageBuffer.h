#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>

#include "support.h"

enum SSBType
{
	NONE_LIST, FLOAT_LIST, INT_LIST, VEC2_LIST, VEC3_LIST, CUSTOM_LIST
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

	void BindBuffer() const;
	void BindBufferBase(GLuint _base = -1) const;
	void UnbindBuffer() const;
	void SetBufferBase(GLuint base);

	void DeleteBuffer();

public:
	GLuint GetID() const { return ssbo_id; }
	GLuint GetBase() const { return ssbo_base; }
	SSBType GetType() const { return ssbo_type; }

public:

	template <typename T>
	void GenStorageBuffer(const std::vector<T>& src);

	template <typename _Point, typename _Sun, typename _Spot>
	void GenStorageBuffers(const std::vector<_Point>& _point, const std::vector<_Sun>& _sun, const std::vector<_Spot>& _spot);
};

template <typename T>
void StorageBuffer::GenStorageBuffer(const std::vector<T>& list)
{
	if (list.size() == 0) return;

	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_STATIC_DRAW);
	UnbindBuffer();
}

template <typename _Point, typename _Sun, typename _Spot>
void StorageBuffer::GenStorageBuffers(const std::vector<_Point>& _point, const std::vector<_Sun>& _sun, const std::vector<_Spot>& _spot)
{
	BindBuffer();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _point.size()*sizeof(_Point) + _sun.size()*sizeof(_Sun) + _spot.size()*sizeof(_Spot), nullptr, GL_STATIC_DRAW);

	void* bufferData = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	size_t offset = 0;
	std::memcpy(static_cast<char*>(bufferData) + offset, _point.data(), _point.size() * sizeof(_Point));
	offset += _point.size() * sizeof(_Point);
	std::memcpy(static_cast<char*>(bufferData) + offset, _sun.data(), _sun.size() * sizeof(_Sun));
	offset += _sun.size() * sizeof(_Sun);
	std::memcpy(static_cast<char*>(bufferData) + offset, _spot.data(), _spot.size() * sizeof(_Spot));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER); 

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_id);

}

