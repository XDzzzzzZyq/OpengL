#include "Environment.h"

Environment::Environment(const std::string& texpath)
{
	o_type = GO_ENVIR;
	envir_shader = RenderShader("pps/Screen", "pps/EnvirBG");

	const bool is_using_HDR = texpath.substr(texpath.find("."), texpath.length()-1)==".hdr";

	Texture rough_tex(texpath, is_using_HDR ? IBL_TEXTURE : RGBA_TEXTURE, GL_MIRRORED_REPEAT);
	envir_IBL_diff.GenIBLDiffuseFrom(rough_tex, true);
	envir_IBL_spec.GenIBLSpecularFrom(rough_tex, true);

	envir_sprite.spr_type = ENVIRN_SPRITE;
	envir_sprite.SetTex();

	//envir_frameBuffer = FrameBuffer(AVAIL_PASSES);

	o_name = "Environment." + std::to_string(GetObjectID());

	envir_shader.InitShader = [&] {
		envir_shader.UseShader();
		envir_shader.SetValue("hdr_texture",		IBL_TEXTURE);
		envir_shader.SetValue("buffer_texture",	BUFFER_TEXTURE + COMBINE_FB);
		envir_shader.SetValue("id_texture",		BUFFER_TEXTURE + ID_FB);
		envir_shader.SetValue("select_texture",	BUFFER_TEXTURE + MASK_FB);
		envir_shader.SetValue("ID_color",			id_color);
		envir_shader.SetValue("RAND_color",		id_color_rand);
		envir_shader.UnuseShader();
	};

	//envir_frameBuffer->UnbindFrameBuffer();	
}

Environment::Environment()
{

}

void Environment::ChangeEnvirTexture(const std::string& texpath) const
{

}

void Environment::ChangeEnvirType(EnvironmentType type) const
{

}

void Environment::BindFrameBuffer() const
{
	//envir_frameBuffer->BindFrameBuffer();
}

void Environment::UnbindFrameBuffer() const
{
	//envir_frameBuffer->UnbindFrameBuffer();
}

void Environment::SwapFrameBuffer(FBType type)
{
	envir_shader.UseShader();
	envir_shader.SetValue("buffer_texture", BUFFER_TEXTURE + type);
}

void Environment::BindEnvironTexture() const
{
	envir_IBL_diff.Bind(IBL_TEXTURE);
	envir_IBL_spec.Bind(IBL_TEXTURE + 1);
}

void Environment::UnbindEnvironTexture() const
{
	envir_IBL_diff.Unbind();
	envir_IBL_spec.Unbind();
}

void Environment::RenderEnvironment(Camera* cam)
{
	envir_shader.UseShader();
	//envir_frameBuffer->BindFrameBufferTex(AVAIL_PASSES);
	envir_IBL_spec.Bind(IBL_TEXTURE);
	//DEBUG(envir_frameBuffer->GetFBCount())

	if (envir_shader.is_shader_changed)
		envir_shader.InitShader();

	if(cam->is_invUniform_changed || envir_shader.is_shader_changed)
		envir_shader.SetValue("cam_rotM", cam->o_Transform);

	if (cam->is_frustum_changed || envir_shader.is_shader_changed) {
		envir_shader.SetValue("cam_fov", glm::radians(cam->cam_pers));
		envir_shader.SetValue("cam_ratio", cam->cam_w / cam->cam_h);
	}

	MeshLib::Square->RenderObjProxy();

	envir_shader.is_shader_changed = false;
}

void Environment::RenderEnvirSpr(Camera* cam)
{
	envir_sprite.RenderSprite(o_position, envir_color, cam);
}
