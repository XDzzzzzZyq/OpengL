#include "Spirit.h"

std::string Spirit::fileroot = "res/tex/spirit/";

Spirit::Spirit()
{
	o_type = GO_SPIRIT;

	SetSpiritShader();

	o_name = "Spirit." + std::to_string(GetObjectID());

}

Spirit::~Spirit()
{
	DeleteSpirit();
}

void Spirit::RenderSpirit(const std::vector<float>& light_data, Camera* cam)
{
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

	MeshLib::Square->RenderObjProxy();
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

	MeshLib::Square->RenderObjProxy();
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

	r_tex->DelTexture();
	r_shader->DelShad();
}

std::string Spirit::ParsePath() const
{
	switch (spr_type)
	{
	case NONE_SPIRIT:
		return Spirit::fileroot + "BAKED.png";break;
	case POINT_LIGHT_SPIRIT:
		return Spirit::fileroot + "light.png";break;
	case SUN_LIGHT_SPIRIT:
		return Spirit::fileroot + "sun-bright.png"; break;
	case SPOT_LIGHT_SPIRIT:
		return Spirit::fileroot + "spot-bright.png"; break;
	case CAM_SPIRIT:
		return Spirit::fileroot + "BAKED.png";break;
	case ENVIRN_SPIRIT:
		return Spirit::fileroot + "cloud.png";break;

	}
	return "???";
	DEBUG("???")
}
