#include "Spirit.h"

std::string Spirit::fileroot = "res/tex/spirit/";

Spirit::Spirit()
{
	static std::vector<float>VertData = {
		-1.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		-1.0f,-1.0f, 0.0f,		0.0f, 1.0f,
		 1.0f,-1.0f, 0.0f,		1.0f, 1.0f
	};

	static std::vector<GLuint> indexArray = std::vector<GLuint>{ 0,2,1,1,2,3 };

	o_type = GO_SPIRIT;
	//std::cout << VertData[100] << std::endl;
	r_vertBuffer = VertexBuffer(VertData.data(), VertData.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(3); //3D position
	layout.Push<float>(2); //UV

	r_vertArry.AddBuffer(r_vertBuffer, layout);
	/*o_verts.Unbind();*/

	r_index = IndexBuffer(indexArray.data(), indexArray.size() * sizeof(GLuint));
	SetSpiritShader();

	o_name = "Spirit." + std::to_string(GetObjectID());
	
}

Spirit::~Spirit()
{
	DeleteSpirit();
}

void Spirit::RenderSpirit(const std::vector<float>& light_data, Camera* cam)
{
	r_vertArry.Bind();
	r_index.Bind();
	r_shader->UseShader();
	r_tex->Bind(SPIRIT_TEXURE);

	//transform settings

	//std::cout << o_Transform;
	if(&light_data)
		r_shader->SetValue("Light_data",6 ,light_data.data(), VEC1_ARRAY);

	if(cam->is_invUniform_changed)
		r_shader->SetValue("U_cam_trans", cam->o_InvTransform);

	if(cam->is_frustum_changed)
		r_shader->SetValue("U_ProjectM", cam->cam_frustum);

	r_shader->SetValue("SpiritOpacity", spirit_opacity);
	r_shader->SetValue("U_Scale", SPIRIT_SIZE);
	//light settings



	glDrawElements(GL_TRIANGLES, r_index.count(), GL_UNSIGNED_INT, nullptr);


	//o_Transform = glm::mat4(1.0f);
	//r_index.Unbind();
	//r_shader->UnuseShader();
	//r_vertArry.Unbind();
	//r_tex->Unbind();


}

void Spirit::RenderSpirit(Camera* cam)
{
	if (cam->is_invUniform_changed)
		r_shader->SetValue("U_cam_trans", cam->o_InvTransform);

	if (cam->is_frustum_changed)
		r_shader->SetValue("U_ProjectM", cam->cam_frustum);

	r_shader->SetValue("SpiritOpacity", spirit_opacity);
	r_shader->SetValue("U_Scale", SPIRIT_SIZE);
	//light settings



	glDrawElements(GL_TRIANGLES, r_index.count(), GL_UNSIGNED_INT, nullptr);
}

void Spirit::SetSpiritShader()
{
	r_shader = RenderShader("SpiritShader");
	r_shader->UseShader();
	r_shader->SetValue("ID_color", id_color);
	r_shader->SetValue("RAND_color", id_color_rand);
	r_shader->UnuseShader();
}

void Spirit::SetTex()
{
	r_tex = Texture(ParsePath(), SPIRIT_TEXURE, GL_REPEAT);

	r_shader->UseShader();
	//o_shader.SetValue("blen", 0.5f);
	r_shader->SetValue("U_color", 1.0f, 0.0f, 1.0f, 1.0f);
	r_shader->SetValue("U_Texture", SPIRIT_TEXURE);
	r_shader->UnuseShader();
}



void Spirit::DeleteSpirit()
{
	r_tex->Unbind();
	r_shader->UnuseShader();
	r_index.Unbind();
	r_vertArry.Unbind();
	r_vertBuffer.Unbind();

	r_tex->DelTexture();
	r_shader->DelShad();
	r_index.DelIndBuff();
	r_vertBuffer.DelVertBuff();
	r_vertArry.DelVertArr();
}

std::string Spirit::ParsePath() const
{
	switch (spr_type)
	{
	case NONE_SPIRIT:
		return Spirit::fileroot + "BAKED.png";break;
	case LIGHT_SPIRIT:
		return Spirit::fileroot + "light.png";break;
	case CAM_SPIRIT:
		return Spirit::fileroot + "BAKED.png";break;
	case ENVIRN_SPIRIT:
		return Spirit::fileroot + "cloud.png";break;

	}
	return "???";
	DEBUG("???")
}
