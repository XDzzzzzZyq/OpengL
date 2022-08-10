#pragma once
#include "GameObject.h"
#include "Transform.h"

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shaders.h"
#include "Camera.h"

#include "Parameters.h"

class DebugLine : public GameObject, public Transform
{
private:
	VertexBuffer dLine_vertBuffer;
	IndexBuffer dLine_index;
	VertexArray dLine_vertArry;
	std::vector<float> dLine_pos_list;

	int vert_count = 2;

	float dLine_width = 2;
	float dLine_opacity = 1.0f;
public:
	bool using_stipple = false, using_smooth = true, multiLine = false;
	glm::vec3 dLine_color = glm::vec3{ 1.0f,1.0f,1.0f };

	mutable Shaders dLine_shader;

	glm::vec3 dLine_start, dLine_end;


	DebugLine(const glm::vec3& start, const glm::vec3& end);
	DebugLine(const std::vector<glm::vec3>& vertices);
	DebugLine();

	~DebugLine();

	void SetDebugLineParas(bool stipple, bool smooth, float width, float opacity);

	void PushDebugLine(const glm::vec3& point);
	void PushDebugLine(float x, float y, float z);
	void PushDebugLines(const std::vector<glm::vec3>& points);

	void RenderDdbugLine(Camera* camera);

	void SetDLineShader();
	void DeleteDLine();

};

