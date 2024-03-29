#pragma once
#include "GameObject.h"
#include "Transform.h"

#include "VertexArray.h"
#include "buffer/IndexBuffer.h"
#include "Shaders.h"
#include "Camera.h"

#include "Parameters.h"

class DebugLine : public GameObject, public Transform3D
{
private:
	std::vector<glm::vec3> dLine_pos_list;

	float dLine_width = 2;
	float dLine_opacity = 1.0f;
public:
	bool using_stipple = false, using_smooth = true, is_multi_lines = false;
	glm::vec3 dLine_color = glm::vec3{ 1.0f,1.0f,1.0f };

	RenderShader dLine_shader;

	glm::vec3 dLine_start, dLine_end;


	DebugLine(const glm::vec3& start, const glm::vec3& end);
	DebugLine(const std::vector<glm::vec3>& vertices);
	DebugLine();

	void SetDebugLineParas(bool stipple, bool smooth, float width, float opacity);

	void PushDebugLine(const glm::vec3& point);
	void PushDebugLine(float x, float y, float z);
	void PushDebugLines(const std::vector<glm::vec3>& points);

	void RenderDdbugLine(Camera* camera);

	void SetDLineShader();
	void* GetShader()		override { return &dLine_shader; };
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }

};

