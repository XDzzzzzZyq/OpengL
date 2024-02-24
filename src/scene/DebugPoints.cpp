#include "DebugPoints.h"
#include "macros.h"

std::vector<float> DebugPoints::VertData = {
	-1.0f, 1.0f, 0.0f,
	 1.0f, 1.0f, 0.0f,
	-1.0f,-1.0f, 0.0f,
	 1.0f,-1.0f, 0.0f
};

DebugPoints::DebugPoints()
	:DebugPoints(std::vector<glm::vec3>{}) {}

DebugPoints::DebugPoints(const std::vector<glm::vec3>& pos_list)
{
	o_type = GO_DP;
	dp_vertBuffer = VertexBuffer(VertData);

	BufferLayout layout;
	layout.Push<float>(3); //3D position

	dp_vertArry.AddBuffer(dp_vertBuffer, layout);

	static std::vector<GLuint> indexArray = std::vector<GLuint>{ 0,2,1,1,2,3 };

	dp_index = IndexBuffer(indexArray.data(), indexArray.size() * sizeof(GLuint));

	dp_pos_list = pos_list;
	dp_pos_buffer = StorageBuffer(VEC3_LIST, 3);

	o_name = "DebugPoints." + std::to_string(GetObjectID());

	SetDebugPointsShader(SQUARE_POINT, true);
}

void DebugPoints::RenderDebugPoint(Camera* camera)
{
	const size_t trans_type = (size_t)is_proj;

	dp_vertArry.Bind();
	dp_index.Bind();
	dp_pos_buffer.BindBufferBase();
	dp_shader[trans_type].UseShader();

	if (dp_shader[trans_type].is_shader_changed)
		dp_shader[trans_type].InitShader();

	if (is_list_changed || dp_shader[trans_type].is_shader_changed)
		dp_shader[trans_type].SetValue("pos_count", (int)dp_pos_list.size());

	dp_shader[trans_type].SetValue("point_color", dp_color);

	dp_shader[trans_type].SetValue("is_selected", (int)is_selected);

	if (is_list_changed || dp_shader[trans_type].is_shader_changed)
		dp_pos_buffer.GenStorageBuffer(dp_pos_list);

	if(camera->is_invUniform_changed || dp_shader[trans_type].is_shader_changed)
		dp_shader[trans_type].SetValue("U_cam_trans", camera->o_InvTransform);

	if(camera->is_frustum_changed || dp_shader[trans_type].is_shader_changed)
		dp_shader[trans_type].SetValue("U_ProjectM", camera->cam_frustum);

	dp_shader[trans_type].SetValue("U_Opacity", dp_opacity);
	dp_shader[trans_type].SetValue("U_Scale", dp_scale);

	glDrawElementsInstanced(GL_TRIANGLES, dp_index.Count(), GL_UNSIGNED_INT, nullptr, dp_pos_list.size());

	//o_Transform = glm::mat4(1.0f);
	//dp_index.Unbind();
	//dp_pos_buffer.UnbindBuffer();
	//dp_shader[trans_type].UnuseShader();
	//dp_vertArry.Unbind();

	is_list_changed = false;
}

void DebugPoints::SetDebugPointsShader(PointType type, bool proj)
{
	dp_type = type;
	is_proj = proj;

	dp_shader[0] = RenderShader("PointsShader");
	dp_shader[1] = RenderShader("PointsShader_proj", "PointsShader");

	dp_shader[0].InitShader = [&] {
		dp_shader[0].UseShader();
		dp_shader[0].SetValue("ID_color", id_color);
		dp_shader[0].SetValue("RAND_color", id_color_rand);
	};

	dp_shader[1].InitShader = [&] {
		dp_shader[1].UseShader();
		dp_shader[1].SetValue("ID_color", id_color);
		dp_shader[1].SetValue("RAND_color", id_color_rand);
	};
}

void DebugPoints::PushDebugPoint(const glm::vec3& point)
{
	dp_pos_list.emplace_back(point);
	is_list_changed = true;
}

void DebugPoints::PushDebugPoint(float x, float y, float z)
{
	PushDebugPoint({x, y, z});
}

void DebugPoints::PushDebugPoints(const std::vector<glm::vec3>& points)
{
	dp_pos_list.reserve(dp_pos_list.size() + points.size());

	LOOP(points.size())
		dp_pos_list.emplace_back(points[i]);
	is_list_changed = true;
}
