#pragma once
#include <vector>
#include <GL/glew.h>
struct BufferElement
{
	GLuint type;
	GLuint count;
	GLuint is_norm;

	static GLuint getTypeSize(GLuint type) {
		switch (type)
		{
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
	}
};


class BufferLayout
{
private:
	std::vector<BufferElement> m_ele_list;
	GLuint m_stride;
public:
	BufferLayout()
		:m_stride(0) {};
	~BufferLayout();

	template<typename T>
	inline constexpr static GLuint ParseType() {
		if constexpr (std::is_same<T, GLfloat>::value) return GL_FLOAT;
		else if      (std::is_same<T, GLuint>::value)  return GL_UNSIGNED_INT;
		else if		 (std::is_same<T, GLubyte>::value) return GL_UNSIGNED_BYTE;
		else										   return GL_NONE;
	}

	template<typename T>
	void Push(GLuint count) {
		//static_assert(false); //add a layout pattern {type, count of element, is_normalized}

		m_ele_list.push_back({ ParseType<T>(), count, GL_FALSE });
		m_stride += sizeof(T) * count; //add up the stride per each Push()
	}

	inline const std::vector<BufferElement>GetEles() const { return m_ele_list; };
	inline GLuint GetStride() const { return m_stride; };
};

