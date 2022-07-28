#include "DebugPoints.h"

DebugPoints::DebugPoints()
{

}

DebugPoints::DebugPoints(const std::vector<float>& pos_list)
{
	dp_vertBuffer = VertexBuffer(VertData.data(), VertData.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(3); //3D position

	dp_vertArry.AddBuffer(dp_vertBuffer, layout);

	std::vector<GLuint>* indexArray = new std::vector<GLuint>{ 0,2,1,1,2,3 };
	GLuint* index = indexArray->data();

	dp_index = IndexBuffer(index, indexArray->size() * sizeof(GLuint));

	dp_pos_list = pos_list;

	o_name = "DebugPoints." + std::to_string(GetObjectID());
}

DebugPoints::~DebugPoints()
{
	DeleteDebugPoints();
}

void DebugPoints::RenderDebugPoint(Camera* camera) const
{
	dp_vertArry.Bind();
	dp_index.Bind();


	dp_shader[(int)is_proj].UseShader();



	//transform settings

	//std::cout << o_Transform;
	dp_shader[(int)is_proj].SetValue("Light_data", dp_pos_list.size(), dp_pos_list.data());
	dp_shader[(int)is_proj].SetValue("U_cam_trans", camera->o_InvTransform);
	dp_shader[(int)is_proj].SetValue("U_ProjectM", camera->cam_frustum);
	dp_shader[(int)is_proj].SetValue("U_Opacity", dp_opacity);
	dp_shader[(int)is_proj].SetValue("U_Scale", dp_scale);
	//light settings


	glDrawElementsInstanced(GL_TRIANGLES, dp_index.count(), GL_UNSIGNED_INT, nullptr, dp_pos_list.size()/3);

	//o_Transform = glm::mat4(1.0f);
	dp_index.Unbind();
	dp_shader[(int)is_proj].UnuseShader();
	dp_vertArry.Unbind();
}

void DebugPoints::SetDebugPointsShader(PointType type, bool proj)
{
	dp_type = type;
	is_proj = proj;

	dp_shader[0] = Shaders("res/shaders/PointsShader.shader");
	dp_shader[1] = Shaders("res/shaders/PointsShadedp_proj.shader");
}

void DebugPoints::PushDebugPoint(const glm::vec3& point)
{
	dp_pos_list.push_back(point[0]);
	dp_pos_list.push_back(point[1]);
	dp_pos_list.push_back(point[2]);
}

void DebugPoints::PushDebugPoints(const std::vector<glm::vec3>& points)
{
	dp_pos_list.reserve(dp_pos_list.size()+points.size()*3);

	LOOP(points.size()){

		dp_pos_list.emplace_back(points[i][0]);
		dp_pos_list.emplace_back(points[i][1]);
		dp_pos_list.emplace_back(points[i][2]);

	}
}

void DebugPoints::DeleteDebugPoints() const
{


		dp_shader[0].UnuseShader();
		dp_shader[1].UnuseShader();
		dp_index.Unbind();
		dp_vertArry.Unbind();
		dp_vertBuffer.Unbind();


		dp_shader[0].DelShad();
		dp_shader[1].DelShad();
		dp_index.DelIndBuff();
		dp_vertBuffer.DelVertBuff();
		dp_vertArry.DelVertArr();

}

