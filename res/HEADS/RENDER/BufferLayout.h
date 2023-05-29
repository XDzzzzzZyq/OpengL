#pragma once
#include <vector>
#include <GL/glew.h>
#include <type_traits>

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
	void Push(GLuint count) {
		//static_assert(false); //add a layout pattern {type, count of element, is_normalized}

		GLuint ele_type = GL_NONE;

		if (std::is_same<T, GLfloat>::value){
			ele_type = GL_FLOAT;
		}else if(std::is_same<T, GLuint>::value){
			ele_type = GL_UNSIGNED_INT;
		}else if(std::is_same<T, GLubyte>::value){
			ele_type = GL_UNSIGNED_BYTE;
		}

		m_ele_list.push_back({ ele_type, count, GL_FALSE });
		m_stride += sizeof(T) * count; //add up the stride per each Push()
	}
/*
	void Push<GLfloat>(GLuint count) {
		m_ele_list.push_back({ GL_FLOAT,count,GL_FALSE });
		m_stride += sizeof(GLfloat) * count; //add up the stride per each Push()
	}

	void Push<GLuint>(GLuint count) {
		m_ele_list.push_back({ GL_UNSIGNED_INT,count,GL_FALSE });
		m_stride += sizeof(GLuint)*count;
	}


	void Push<unsigned char>(GLuint count) {
		m_ele_list.push_back({ GL_UNSIGNED_BYTE,count,GL_FALSE });
		m_stride += sizeof(GLbyte) * count;
	}*/
	inline const std::vector<BufferElement>GetEles() const { return m_ele_list; };
	inline GLuint GetStride() const { return m_stride; };
};

