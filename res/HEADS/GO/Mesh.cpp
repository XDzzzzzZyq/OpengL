#include "Mesh.h"

Mesh::Mesh(const std::string& path)
{
	o_type = GO_MESH;

	o_mesh = MeshLib::LoadMesh(path);

	o_name = o_mesh->GetMeshName();

	o_material = std::make_shared<Material>();
}

Mesh::Mesh()
{
	DEBUG("mesh c")
}

Mesh::~Mesh()
{
	DeleteObj();
}

void Mesh::RenderMesh(const Camera* cam)
{
	o_shader->UseShader();

	if (o_material->is_mat_struct_changed && using_material)
		o_shader->UpdateMaterial(o_material.get());

	if (o_shader->is_shader_changed)
		o_shader->InitShader();

	if (is_Uniform_changed || o_shader->is_shader_changed)
		o_shader->SetValue("U_obj_trans", o_Transform);

	if (cam->is_invUniform_changed || o_shader->is_shader_changed)
		o_shader->SetValue("U_cam_trans", cam->o_InvTransform);

	if (cam->is_frustum_changed || o_shader->is_shader_changed)
		o_shader->SetValue("U_ProjectM", cam->cam_frustum);

	if (cam->is_Uniform_changed || cam->is_frustum_changed || o_shader->is_shader_changed)
		o_shader->SetValue("Scene_data", 8, cam->cam_floatData.data(), VEC1_ARRAY);

	o_shader->SetValue("is_selected", (int)is_selected);

	if (using_material)
		o_shader->SetValue(o_material.get());

	if (using_material)
		o_material->BindMatTexture();

	RenderObjProxy();

#if 1
	o_shader->UnuseShader();
#endif


}

void Mesh::RenderObjProxy() const
{
	o_mesh->RenderObjProxy();
}

void Mesh::SetObjShader(std::string vert, std::string frag)
{
	o_shader = std::make_shared<RenderShader>(vert, frag);
	//std::cout << "Shader:" << (glGetError()) << "\n";
	o_shader->UseShader();
	//matrix = glm::translate(matrix, o_position);
	o_shader->SetValue("U_ProjectM", o_Transform);
	o_shader->SetValue("ID_color", id_color);

	o_shader->InitShader = [&] {
		o_shader->UseShader();
		o_shader->SetValue("RAND_color", id_color_rand);
		o_shader->SetValue("U_ProjectM", o_Transform);
		o_shader->SetValue("ID_color", id_color);

		o_shader->UnuseShader();
	};

	if (using_material)
		o_shader->UpdateMaterial(o_material.get());
}

void Mesh::SetTex(MatParaType _type, std::string _name)
{
	o_material->SetMatParam(_type, TextureLib::LoadTexture(_name));
}

void Mesh::SetMatColor(MatParaType _type, float _val)
{
	o_material->SetMatParam(_type, _val);
}

void Mesh::SetMatColor(MatParaType _type, glm::vec3 _col)
{
	o_material->SetMatParam(_type, _col);
}

void Mesh::SetCenter()
{
	SetPos(-o_mesh->GetMeshCenter());
}

void Mesh::DeleteObj()
{
	if (o_shader)o_shader->UnuseShader();

	if (o_shader)o_shader->DelShad();
}
