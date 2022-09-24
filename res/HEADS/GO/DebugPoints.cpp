#include "DebugPoints.h"

DebugPoints::DebugPoints()
{
	o_type = GO_DP;
	dp_vertBuffer = VertexBuffer(VertData.data(), VertData.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(3); //3D position

	dp_vertArry.AddBuffer(dp_vertBuffer, layout);

	std::vector<GLuint>* indexArray = new std::vector<GLuint>{ 0,2,1,1,2,3 };
	GLuint* index = indexArray->data();

	dp_index = IndexBuffer(index, indexArray->size() * sizeof(GLuint));

	o_name = "DebugPoints." + std::to_string(GetObjectID());

	SetDebugPointsShader(SQUARE_POINT, true);
}

DebugPoints::DebugPoints(const std::vector<float>& pos_list)
{
	o_type = GO_DP;
	dp_vertBuffer = VertexBuffer(VertData.data(), VertData.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(3); //3D position

	dp_vertArry.AddBuffer(dp_vertBuffer, layout);

	std::vector<GLuint>* indexArray = new std::vector<GLuint>{ 0,2,1,1,2,3 };
	GLuint* index = indexArray->data();

	dp_index = IndexBuffer(index, indexArray->size() * sizeof(GLuint));

	dp_pos_list = pos_list;

	o_name = "DebugPoints." + std::to_string(GetObjectID());

	SetDebugPointsShader(SQUARE_POINT, true);
}

DebugPoints::~DebugPoints()
{
	DeleteDebugPoints();
}

void DebugPoints::RenderDebugPoint(Camera* camera)
{

	
	dp_vertArry.Bind();
	dp_index.Bind();
	dp_pos_buffer.BindBuffer();
	dp_shader[(int)is_proj]->UseShader();
	
	if (dp_shader[(int)is_proj]->is_shader_changed)
		dp_shader[(int)is_proj]->InitShader();

	if (is_list_changed || dp_shader[(int)is_proj]->is_shader_changed)
		dp_shader[(int)is_proj]->SetValue("pos_count", dp_pos_list.size()/3);	

	dp_shader[(int)is_proj]->SetValue("point_color", dp_color);

	dp_shader[(int)is_proj]->SetValue("is_selected", (int)is_selected);

	if (is_list_changed || dp_shader[(int)is_proj]->is_shader_changed) {
		dp_pos_buffer.GenStorageBuffer(dp_pos_list);
		dp_shader[(int)is_proj]->SetValue("testList", dp_pos_list.size(), dp_pos_list.data(), VEC3_ARRAY);
	}
		

	if(camera->is_invUniform_changed || dp_shader[(int)is_proj]->is_shader_changed)
		dp_shader[(int)is_proj]->SetValue("U_cam_trans", camera->o_InvTransform);	

	if(camera->is_frustum_changed || dp_shader[(int)is_proj]->is_shader_changed)
		dp_shader[(int)is_proj]->SetValue("U_ProjectM", camera->cam_frustum);	

	dp_shader[(int)is_proj]->SetValue("U_Opacity", dp_opacity);	
	dp_shader[(int)is_proj]->SetValue("U_Scale", dp_scale);	

	glDrawElementsInstanced(GL_TRIANGLES, dp_index.count(), GL_UNSIGNED_INT, nullptr, dp_pos_list.size()/3);

	//o_Transform = glm::mat4(1.0f);
	//dp_index.Unbind();
	//dp_pos_buffer.UnbindBuffer();
	//dp_shader[(int)is_proj]->UnuseShader();
	//dp_vertArry.Unbind();

	is_list_changed = false;
}

void DebugPoints::SetDebugPointsShader(PointType type, bool proj)
{
	dp_type = type;
	is_proj = proj;

	dp_shader[0] = Shaders("PointsShader");
	dp_shader[1] = Shaders("PointsShader_proj");
	
	dp_shader[0]->InitShader = [&] {
		dp_shader[0]->UseShader();
		dp_shader[0]->SetValue("ID_color", id_color);
		dp_shader[0]->SetValue("RAND_color", id_color_rand);
	};

	dp_shader[1]->InitShader = [&] {
		dp_shader[1]->UseShader();
		dp_shader[1]->SetValue("ID_color", id_color);
		dp_shader[1]->SetValue("RAND_color", id_color_rand);
	};
}

void DebugPoints::PushDebugPoint(const glm::vec3& point)
{
	dp_pos_list.push_back(point[0]);
	dp_pos_list.push_back(point[1]);
	dp_pos_list.push_back(point[2]);
	is_list_changed = true;
}

void DebugPoints::PushDebugPoint(float x, float y, float z)
{
	dp_pos_list.push_back(x);
	dp_pos_list.push_back(y);
	dp_pos_list.push_back(z);
	is_list_changed = true;
}

void DebugPoints::PushDebugPoints(const std::vector<glm::vec3>& points)
{
	dp_pos_list.reserve(dp_pos_list.size()+points.size()*3);

	LOOP(points.size()){

		dp_pos_list.emplace_back(points[i][0]);
		dp_pos_list.emplace_back(points[i][1]);
		dp_pos_list.emplace_back(points[i][2]);

	}
	is_list_changed = true;
}

void DebugPoints::DeleteDebugPoints() const
{


		dp_shader[0]->UnuseShader();
		dp_shader[1]->UnuseShader();
		dp_index.Unbind();
		dp_vertArry.Unbind();
		dp_vertBuffer.Unbind();


		dp_shader[0]->DelShad();
		dp_shader[1]->DelShad();
		dp_index.DelIndBuff();
		dp_vertBuffer.DelVertBuff();
		dp_vertArry.DelVertArr();

}

