#pragma once

#include "GameObject.h"
#include "VertexArray.h"
#include "buffer/IndexBuffer.h"
#include "Shaders.h"
#include "Transform.h"

#include "buffer/StorageBuffer.h"

#include "Camera.h"
#include <optional>

enum PointType {
	SQUARE_POINT, RHOMBUS_POINT, CIR_POINT, CUBE_POINT
};

class DebugPoints : public GameObject, public Transform3D
{
private:
	VertexArray dp_vertArry;
	VertexBuffer dp_vertBuffer;
	IndexBuffer dp_index;
	StorageBuffer dp_pos_buffer;

	static std::vector<float> VertData;

public:
	PointType dp_type = SQUARE_POINT;
	bool is_proj = true;
	bool is_list_changed = true;
	bool is_scaled;
	mutable std::optional<RenderShader> dp_shader[2]; //no proj | using proj

	mutable float dp_scale = 1.0f;
	float dp_opacity = 1.0f;
	glm::vec3 dp_color = glm::vec3(1.0f, 1.0f, 1.0f);
	std::vector<glm::vec3> dp_pos_list;

	DebugPoints();
	DebugPoints(const std::vector<glm::vec3>& pos_list);
	~DebugPoints();

	void RenderDebugPoint(Camera* camera);

	void SetDebugPointsShader(PointType type, bool proj);
	void PushDebugPoint(const glm::vec3& point);
	void PushDebugPoint(float x, float y, float z);
	void PushDebugPoints(const std::vector<glm::vec3>& points);

	void* GetShader()		override { return &dp_shader[is_proj].value(); };
	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }

	void DeleteDebugPoints() const;
};

