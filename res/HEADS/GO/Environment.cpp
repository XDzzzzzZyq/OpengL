#include "Environment.h"

Environment::Environment(const std::string& texpath)
{

	static std::vector<float> screenQuad = {
		// positions		// texCoords
		-1.0f, 1.0f,	0.0f, 1.0f,
		 1.0f, 1.0f,	1.0f, 1.0f,
		-1.0f,-1.0f,	0.0f, 0.0f,
		 1.0f,-1.0f,	1.0f, 0.0f
	};
	static auto indexArray = std::vector<GLuint>{ 0,2,1,1,2,3 };

	o_type = GO_ENVIR;
	envir_shader = RenderShader("Screen", "EnvirBG");	

	const bool is_using_HDR = texpath.substr(texpath.find("."), texpath.length()-1)==".hdr";

	Texture rough_tex(texpath, is_using_HDR ? IBL_TEXTURE : RGBA_TEXTURE, GL_MIRRORED_REPEAT);
	envir_IBL_diff.GenIBLDiffuseFrom(rough_tex, true);
	envir_IBL_spec.GenIBLSpecularFrom(rough_tex, true);
	rough_tex.DelTexture();

	envir_spirit.spr_type = ENVIRN_SPIRIT;
	envir_spirit.SetTex();

	//envir_frameBuffer = FrameBuffer(AVAIL_PASSES);

	o_name = "Environment." + std::to_string(GetObjectID());

	o_vertBuffer = VertexBuffer(screenQuad.data(), screenQuad.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(2); //2D position
	layout.Push<float>(2); //UV

	o_vertArry.AddBuffer(o_vertBuffer, layout);

	GLuint* index = indexArray.data();

	o_indexBuffer = IndexBuffer(index, indexArray.size() * sizeof(GLuint)); 

	envir_shader->InitShader = [&] {
		envir_shader->UseShader();
		envir_shader->SetValue("hdr_texture",		IBL_TEXTURE);
		envir_shader->SetValue("buffer_texture",	BUFFER_TEXTURE + COMBINE_FB);
		envir_shader->SetValue("id_texture",		BUFFER_TEXTURE + ID_FB);
		envir_shader->SetValue("select_texture",	BUFFER_TEXTURE + MASK_FB);
		envir_shader->SetValue("ID_color",			id_color);
		envir_shader->SetValue("RAND_color",		id_color_rand);
		envir_shader->UnuseShader();
	};

	//envir_frameBuffer->UnbindFrameBuffer();	
}

Environment::Environment()
{

}

Environment::~Environment()
{
	envir_shader->DelShad();
	envir_IBL_diff.DelTexture();
	//envir_frameBuffer->Del();
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
	envir_shader->UseShader();
	envir_shader->SetValue("buffer_texture", BUFFER_TEXTURE + type);
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

void Environment::GenFloatData() const
{

}

void Environment::RenderEnvironment(Camera* cam)
{
	o_vertArry.Bind();
	envir_shader->UseShader();
	o_indexBuffer.Bind();
	//envir_frameBuffer->BindFrameBufferTex(AVAIL_PASSES);
	envir_IBL_spec.Bind(IBL_TEXTURE);
	//DEBUG(envir_frameBuffer->GetFBCount())

	if (envir_shader->is_shader_changed)
		envir_shader->InitShader();

	if(cam->is_invUniform_changed || envir_shader->is_shader_changed)
		envir_shader->SetValue("cam_rotM", cam->o_rotMat);

	if (cam->is_frustum_changed || envir_shader->is_shader_changed) {
		envir_shader->SetValue("cam_fov", glm::radians(cam->cam_pers));
		envir_shader->SetValue("cam_ratio", cam->cam_w / cam->cam_h);
	}

	envir_shader->is_shader_changed = false;
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Environment::RenderEnvirSpr(Camera* cam)
{
	envir_spirit.RenderSpirit(vec3_stdVec6(o_position, envir_color), cam);
}
