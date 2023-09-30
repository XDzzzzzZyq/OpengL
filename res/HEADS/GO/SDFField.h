#pragma once

#include "GameObject.h"
#include "Transform.h"
#include "StorageBuffer.h"
#include "Camera.h"
#include "Shaders.h"

class SDFField :  public GameObject, public Transform3D
{

public:

	// width: x-axis, depth: y-axis, height: z-axis
	GLuint sdf_width = 0, sdf_depth = 0, sdf_height = 0;
	GLuint sdf_subdiv = 1;

private:

	struct SDFInfo {
		alignas(16) glm::vec3 pos;
		alignas(16) glm::vec3 scale;
		alignas(16) glm::vec3 size;
		GLuint subdiv;
	};

	static ChainedShader sdf_shader;

public:

	SDFField();
	SDFField(GLuint width, GLuint depth, GLuint height);
	~SDFField();

private:

	StorageBuffer sdf_data;

public:

	void ResetBuffer();
	void ResetDistance();
	void Resize(GLuint width, GLuint depth, GLuint height);
	void Bind(GLuint _base = -1);
	void Unbind();

	void BindShader();
	void BindTargetTrans(const glm::mat4& _trans);
	void UnbindShader();

public:

	void Subdivide(GLuint _iter);
	std::vector<float> ReadSDF();
	float ReadSDFAt(GLuint x, GLuint y, GLuint z);
	void LoadSDF(const std::vector<float>& _data);

	GLuint GetSDFIndex(GLuint x, GLuint y, GLuint z);

public:

	void RenderSDF(const Camera* cam);

public:

	// only for debug
	void SDFLinearGrad();
	void SDFRadialGrad();
};

