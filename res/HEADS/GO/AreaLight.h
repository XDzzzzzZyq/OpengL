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

class AreaLight : public GameObject, public Transform3D
{
private:
	VertexArray o_vertArray;
	VertexBuffer o_vertBuffer;
	IndexBuffer o_index;

	glm::vec3 light_color;

public:

	mutable std::optional<RenderShader> o_shader;
	AreaLight(std::vector<float> &VertData, const glm::vec3 &light_color);
	~AreaLight();

	void RenderPolygon(Camera* cam);
	void SetPolygonShader();
	void DeletePolygon();
};
