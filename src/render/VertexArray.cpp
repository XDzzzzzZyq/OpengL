#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(3, &m_renderID);
}

VertexArray::~VertexArray()
{
	//
}

void VertexArray::AddBuffer(VertexBuffer& vb, BufferLayout bl)
{
	vb.Bind();
	Bind();

	const auto& eles = bl.GetEles(); //get the list of layout patterns
	GLuint offset = 0;

	for (GLuint i = 0;i < eles.size();i++)
	{
		const auto& ele = eles[i];
		//allocate all the elements in vertexArray

		glEnableVertexAttribArray(i);
		                //attrib id//ele cont//ele type//is_norm//size per vertex//offset per attrib
		glVertexAttribPointer(i, ele.count, ele.type, ele.is_norm, bl.GetStride(), (const void*)offset);
		offset += ele.count * BufferElement::getTypeSize(ele.type);
	}
	vb.Unbind();
	Unbind();

	m_stride = bl.GetStride();
}

void VertexArray::DelVertArr() const
{
	glDeleteVertexArrays(3, &m_renderID);
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_renderID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}
