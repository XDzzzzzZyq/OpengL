#include "SDFField.h"
#include "xdz_math.h"
#include "macros.h"

ChainedShader SDFField::sdf_shader = {};

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
	sdf_shader = ChainedShader::ImportShader("Empty.vert", "SDF_Construct.geom");

	ResetBuffer();
}

SDFField::~SDFField()
{

}

void SDFField::ResetBuffer()
{
	assert(sdf_width * sdf_depth * sdf_height > 0);

	std::vector<GLuint> buffer(sdf_width * sdf_depth * sdf_height, 999);
	SDFInfo info(o_position, o_scale, glm::vec3{ sdf_width, sdf_depth, sdf_height }, sdf_subdiv);

	sdf_data.GenStorageBuffers(info, buffer);
}

void SDFField::ResetDistance()
{
	static ComputeShader& reset_distance = ComputeShader::ImportShader("Reset_Distance", Uni("default_dist", 999.0f));
	reset_distance.UseShader();
	reset_distance.RunComputeShader(sdf_width, sdf_depth, sdf_height);
	reset_distance.UnuseShader();
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

void SDFField::BindShader()
{
	sdf_shader.UseShader();
	sdf_shader.SetValue("U_offset", glm::vec3(0));
}

void SDFField::BindTargetTrans(const glm::mat4& _trans)
{
	sdf_shader.SetValue("U_model", _trans);
}

void SDFField::UnbindShader()
{
	sdf_shader.UnuseShader();
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
	sdf_data.ReadStorageBuffer(result, sizeof(SDFInfo));

	return result;
}

float SDFField::ReadSDFAt(GLuint x, GLuint y, GLuint z)
{
	return -1;
}

void SDFField::LoadSDF(const std::vector<float>& _data)
{

}

GLuint SDFField::GetSDFIndex(GLuint x, GLuint y, GLuint z)
{
	return x + y * sdf_width + z * sdf_depth * sdf_width;
}

void SDFField::RenderSDF(const Camera* cam)
{

}


GLuint FloatFlip3(float fl)
{
	GLuint f = *(GLuint*)(&fl);
	return (f << 1) | (f >> 31);		//Rotate sign bit to least significant
}
float IFloatFlip3(GLuint f2)
{
	GLuint u = (f2 >> 1) | (f2 << 31);
	return *(float*)(&u);
}
void SDFField::SDFLinearGrad()
{

}

void SDFField::SDFRadialGrad()
{
	std::vector<GLuint> buffer(sdf_width * sdf_depth * sdf_height);
	SDFInfo info(o_position, o_scale, glm::vec3{ sdf_width, sdf_depth, sdf_height }, sdf_subdiv);

	LOOP_N(sdf_width, x) {
		LOOP_N(sdf_depth, y) {
			LOOP_N(sdf_height, z) {
				GLuint index = GetSDFIndex(x, y, z);
				glm::vec3 center = 0.5f * info.size - 0.5f;
				glm::vec3 dir = glm::vec3(x, y, z) - center;
				buffer.data()[index] = FloatFlip3(glm::length(dir) / glm::length(center));
			}
		}
	}

	sdf_data.GenStorageBuffers(info, buffer);
}
