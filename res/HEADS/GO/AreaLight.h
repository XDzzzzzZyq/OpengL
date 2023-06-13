#pragma once

#include <optional>

#include "GL/glew.h"

#include "VertexArray.h"
#include "IndexBuffer.h"

#include "Texture.h"
#include "Shaders.h"

#include "GameObject.h"
#include "Camera.h"

#include "Transform.h"

class AreaLight : public GameObject, public Transform3D
{
private:
	VertexArray o_vertArray;
	VertexBuffer o_vertBuffer;
	IndexBuffer o_index;

public:
	std::vector<float> verts;
	glm::vec3 light_color;
	float light_power;
	bool use_shadow;

	mutable std::optional<RenderShader> o_shader;
	AreaLight(const std::vector<float> &verts, const glm::vec3 &light_color = {1.0f, 1.0f, 1.0f}, float light_power = 1.0f, bool useShadow = true);
	~AreaLight();

	void RenderPolygon(Camera* cam);
	void SetPolygonShader();
	void DeletePolygon();
};
