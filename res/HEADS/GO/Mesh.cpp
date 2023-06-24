#include "Mesh.h"

Mesh::Mesh(const std::string& path)
{
	o_type = GO_MESH;

	o_mesh = MeshLib::LoadMesh(path);

	o_name = o_mesh->GetMeshName();

}

Mesh::Mesh()
{
	DEBUG("mesh c")
}

Mesh::~Mesh()
{
	DeleteObj();
}

void Mesh::RenderObj(Camera* cam)
{
	o_shader->UseShader();

	if (o_tex)
		o_tex->Bind();

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

	RenderObjProxy();

	//o_Transform = glm::mat4(1.0f);

#if 1
	o_shader->UnuseShader();

	if (o_tex)
		o_tex->Unbind();
#endif


}

void Mesh::RenderObjProxy() const
{
	o_mesh->RenderObjProxy();
}

void Mesh::SetObjShader(std::string vert, std::string frag)
{
	o_shader = RenderShader(vert, frag);
	//std::cout << "Shader:" << (glGetError()) << "\n";
	o_shader->UseShader();
	//matrix = glm::translate(matrix, o_position);
	o_shader->SetValue("U_ProjectM", o_Transform);
	o_shader->SetValue("ID_color", id_color);

	o_shader->InitShader = [&] {
		o_shader->UseShader();

		o_shader->SetValue("blen", 0.5f);
		o_shader->SetValue("U_color", 1.0f, 0.0f, 1.0f, 1.0f);
		o_shader->SetValue("Envir_Texture", IBL_TEXTURE);
		o_shader->SetValue("Envir_Texture_diff", IBL_TEXTURE + 1);
		o_shader->SetValue("RAND_color", id_color_rand);
		o_shader->SetValue("U_ProjectM", o_Transform);
		o_shader->SetValue("ID_color", id_color);

		if (o_tex)
			o_shader->SetValue("U_Texture", o_tex->tex_type + o_tex->tex_slot_offset);

		o_shader->UnuseShader();
	};
}

void Mesh::SetTex(std::string _path, TextureType _type)
{
	o_tex = Texture(_path, _type, GL_REPEAT);
}

void Mesh::SetCenter()
{
	SetPos(-o_mesh->GetMeshCenter());
}

void Mesh::SetShadow(bool _shadow)
{
	using_shadow = _shadow;
}

void Mesh::DeleteObj()
{
	if (o_tex)o_tex->Unbind();
	if (o_shader)o_shader->UnuseShader();

	if (o_tex)o_tex->DelTexture();
	if (o_shader)o_shader->DelShad();
}
