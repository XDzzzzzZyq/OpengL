#pragma once

#include "GL/glew.h"

#include "VertexArray.h"
#include "IndexBuffer.h"

#include "Texture.h"
#include "Shaders.h"

#include "GameObject.h"
#include "Camera.h"
#include "Light.h"

#include "Transform.h"


class Mesh : public GameObject/* ,public Shaders*/, public Transform
{
private:
	Reading read;
	float* VertData;
	VertexArray o_vertArry;
	VertexBuffer o_vertBuffer;
	IndexBuffer o_index;
	Texture o_tex;

	glm::vec3 center=glm::vec3(0.0f);
public:

	mutable Shaders o_shader;
	Mesh(const char* path);
	Mesh();
	~Mesh();

	void RenderObj(const Camera& cam, const std::unordered_map<int, Light*>& light_list);

	void SetObjShader(std::string path);
	void SetTex(std::string path, TextureType slot);
	void SetCenter();

	void Report() {
		DEBUG(1);
	}
	
	void DeleteObj();
};

