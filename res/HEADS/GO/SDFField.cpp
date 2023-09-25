#include "SDFField.h"
#include "xdz_math.h"
#include "macros.h"

SDFField::SDFField()
{

}

// width: x-axis, depth: y-axis, height: z-axis
SDFField::SDFField(GLuint width, GLuint height, GLuint depth)
	: sdf_width(width), sdf_depth(depth), sdf_height(height)
{
	o_type = GO_SDFFIELD;
	o_name = "SDF Field";

	sdf_data = StorageBuffer(FLOAT_LIST, 6);

	ResetBuffer();
}

SDFField::~SDFField()
{

}

void SDFField::ResetBuffer()
{
	assert(sdf_width * sdf_depth * sdf_height > 0);

	std::vector<float> buffer(sdf_width * sdf_depth * sdf_height, -999);
	sdf_data.GenStorageBuffer(buffer);
}

// width: x-axis, depth: y-axis, height: z-axis
void SDFField::Resize(GLuint width, GLuint depth, GLuint height)
{
	sdf_width = width;
	sdf_depth = depth;
	sdf_height = height;

	ResetBuffer();
}

void SDFField::Bind(GLuint _base /*= -1*/)
{
	if (_base != -1)
		sdf_data.SetBufferBase(_base);

	sdf_data.BindBufferBase();
}

void SDFField::Unbind()
{
	sdf_data.UnbindBuffer();
}

void SDFField::Subdivide(GLuint _iter)
{
	assert(_iter > 0);

	sdf_subdiv = _iter;

	LOOP(_iter) {
	
	}

	ResetBuffer();
}

std::vector<float> SDFField::ReadSDF()
{
	assert(sdf_width * sdf_depth * sdf_height > 0);

	std::vector<float> result(sdf_width * sdf_depth * sdf_height);
	sdf_data.ReadStorageBuffer(result);

	DEBUG(result);

	return result;
}

float SDFField::ReadSDFAt(GLuint x, GLuint y, GLuint z)
{
	return -1;
}

void SDFField::LoadSDF(const std::vector<float>& _data)
{

}

void SDFField::RenderSDF(const Camera* cam)
{

}
