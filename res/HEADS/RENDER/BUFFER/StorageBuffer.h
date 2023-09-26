#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <cstring>

enum SSBType
{
	NONE_LIST, FLOAT_LIST, INT_LIST, VEC2_LIST, VEC3_LIST, CUSTOM_LIST
};

template<typename T>
struct is_not_vector : std::true_type {};

template<typename T, typename C>
struct is_not_vector<std::vector<T, C>> : std::false_type {};

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

	template <typename T>
	void ReadStorageBuffer(std::vector<T>& tar, GLuint _offset = 0);

	template <typename _Info, typename _Ele> requires is_not_vector<_Info>::value
	void GenStorageBuffers(const _Info& _info, const std::vector<_Ele>& _data);

};

template <typename _Info, typename _Ele> requires is_not_vector<_Info>	::value
void StorageBuffer::GenStorageBuffers(const _Info& _info, const std::vector<_Ele>& _data)
{
	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(_Info) + _data.size()*sizeof(_Ele), nullptr, GL_STATIC_DRAW);

	void* bufferData = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	std::memcpy(static_cast<char*>(bufferData), &_info, sizeof(_Info));
	std::memcpy(static_cast<char*>(bufferData) + sizeof(_Info), _data.data(), _data.size() * sizeof(_Ele));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	UnbindBuffer();
}

template <typename T>
void StorageBuffer::GenStorageBuffer(const std::vector<T>& list)
{
	if (list.size() == 0) return;

	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_STATIC_DRAW);
	UnbindBuffer();
}

template <typename T>
void StorageBuffer::ReadStorageBuffer(std::vector<T>& tar, GLuint _offset)
{
	BindBuffer();
	T* dataPtr = static_cast<T*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

	if (dataPtr == nullptr)
		return;

	GLint bufferSize = tar.size();

	if (bufferSize == 0) {
		glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		tar.resize(bufferSize);
	}

	std::memcpy(tar.data(), dataPtr, bufferSize * sizeof(T));

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	UnbindBuffer();
}

