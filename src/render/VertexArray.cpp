#include "VertexArray.h"

void VertexArray::_delVAO()
{
	glDeleteVertexArrays(3, &vao_id);
	vao_id = 0;
}

VertexArray::VertexArray()
{
	glGenVertexArrays(3, &vao_id);
}

VertexArray::VertexArray(const VertexArray& vao)
{
	_resetVAOID(vao.GetVertArrayID());
	vao_stride = vao.vao_stride;
}

VertexArray::VertexArray(VertexArray&& vao) noexcept
{
	_resetVAOID(vao.GetVertArrayID());
	vao.vao_id = 0;
	vao_stride = vao.vao_stride;
}

VertexArray::~VertexArray()
{
	if (GetVertArrayID() != 0)
		_delVAO();
}

VertexArray& VertexArray::operator=(const VertexArray& vao)
{
	_resetVAOID(vao.GetVertArrayID());
	vao_stride = vao.vao_stride;

	return *this;
}

VertexArray& VertexArray::operator=(VertexArray&& vao) noexcept
{
	_resetVAOID(vao.GetVertArrayID());
	vao.vao_id = 0;
	vao_stride = vao.vao_stride;

	return *this;
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

	vao_stride = bl.GetStride();
}

void VertexArray::Bind() const
{
	glBindVertexArray(vao_id);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}
