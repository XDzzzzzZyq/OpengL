#include "Sprite.h"
#include "MeshData.h"
#include "Camera.h"

#include "xdz_math.h"

std::string Sprite::fileroot = "res/tex/sprite/";

Sprite::Sprite()
{
	o_type = GO_SPRITE;

	SetSpriteShader();

	o_name = "Sprite." + std::to_string(GetObjectID());

}

void Sprite::RenderSprite(const Context& ctx, const glm::vec3& pos, const glm::vec3& col, int id)
{
	const Camera* cam = dynamic_cast<const Camera*>(ctx.scene.GetActiveCamera());
	if (!cam) return;
	
	spr_shader.UseShader();
	spr_tex.Bind(SPIRIT_TEXURE);

	// transform settings

	spr_shader.SetValue("U_pos", pos);
	spr_shader.SetValue("U_col", col);
	spr_shader.SetValue("ID_color", xdzm::get_id_color(id));
	spr_shader.SetValue("RAND_color", xdzm::get_random_color(id));

	if(cam->is_invUniform_changed)
		spr_shader.SetValue("U_cam_trans", cam->o_InvTransform);

	if(cam->is_frustum_changed)
		spr_shader.SetValue("U_ProjectM", cam->cam_frustum);

	// light settings

	spr_shader.SetValue("SpriteOpacity", spr_opacity);
	spr_shader.SetValue("U_Scale", SPRITE_SIZE);

	MeshLib::Square->RenderObjProxy();
}

void Sprite::RenderSprite(const Context& ctx)
{
	const Camera* cam = dynamic_cast<const Camera*>(ctx.scene.GetActiveCamera());
	if (!cam) return;
	
	if (cam->is_invUniform_changed)
		spr_shader.SetValue("U_cam_trans", cam->o_InvTransform);

	if (cam->is_frustum_changed)
		spr_shader.SetValue("U_ProjectM", cam->cam_frustum);

	spr_shader.SetValue("SpriteOpacity", spr_opacity);
	spr_shader.SetValue("U_Scale", SPRITE_SIZE);
	//light settings

	MeshLib::Square->RenderObjProxy();
}

void Sprite::SetSpriteShader()
{
	spr_shader = RenderShader("SpriteShader");
	spr_shader.UseShader();
	spr_shader.SetValue("ID_color", xdzm::get_id_color(GetObjectID()));
	spr_shader.SetValue("RAND_color", xdzm::get_random_color(GetObjectID()));
	spr_shader.UnuseShader();
}

void Sprite::SetTex()
{
	spr_tex = Texture(ParsePath(), SPIRIT_TEXURE, GL_REPEAT);

	spr_shader.UseShader();
	//o_shader.SetValue("blen", 0.5f);
	spr_shader.SetValue("U_color", 1.0f, 0.0f, 1.0f, 1.0f);
	spr_shader.SetValue("U_Texture", SPIRIT_TEXURE);
	spr_shader.UnuseShader();
}

std::string Sprite::ParsePath() const
{
	switch (spr_type)
	{
	case NONE_SPRITE:
		return Sprite::fileroot + "BAKED.png";
	case POINT_LIGHT_SPRITE:
		return Sprite::fileroot + "light.png";
	case SUN_LIGHT_SPRITE:
		return Sprite::fileroot + "sun-bright.png";
	case SPOT_LIGHT_SPRITE:
		return Sprite::fileroot + "spot-bright.png";
	case CAM_SPRITE:
		return Sprite::fileroot + "BAKED.png";
	case ENVIRN_SPRITE:
		return Sprite::fileroot + "cloud.png";
	}
	return "???";
}
