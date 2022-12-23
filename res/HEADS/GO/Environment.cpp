#include "Environment.h"

Environment::Environment(const std::string& texpath)
{

	o_type = GO_ENVIR;
	envir_shader = Shaders("IBRShader");	

	const bool is_using_HDR = texpath.substr(texpath.find("."), texpath.length()-1)==".hdr";

	envir_IBL_spec = Texture(texpath, is_using_HDR ? IBL_TEXTURE : RGBA_TEXTURE, GL_REPEAT);
	envir_IBL_spec.Bind(); 
	//envir_IBL_diff.GenIrradiaceConvFrom(envir_IBL_spec.GetTexID());

	envir_spirit.spr_type = ENVIRN_SPIRIT;
	envir_spirit.SetTex();

	envir_frameBuffer = FrameBuffer(4, COMBINE_FB, ID_FB, RAND_FB, ALPHA_FB);	

	o_name = "Environment." + std::to_string(GetObjectID());

	o_vertBuffer = VertexBuffer(screenQuad.data(), screenQuad.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(2); //2D position
	layout.Push<float>(2); //UV

	o_vertArry.AddBuffer(o_vertBuffer, layout);

	auto* indexArray = new std::vector<GLuint>{ 0,2,1,1,2,3 };
	GLuint* index = indexArray->data();

	o_indexBuffer = IndexBuffer(index, indexArray->size() * sizeof(GLuint)); 

	envir_shader->InitShader = [&] {
		envir_shader->UseShader();
		envir_shader->SetValue("hdr_texture",		IBL_TEXTURE);
		envir_shader->SetValue("screen_texture",	BUFFER_TEXTURE + COMBINE_FB);
		envir_shader->SetValue("id_texture",		BUFFER_TEXTURE + ID_FB);
		envir_shader->SetValue("select_texture",	BUFFER_TEXTURE + ALPHA_FB);
		envir_shader->SetValue("ID_color", id_color);
		envir_shader->SetValue("RAND_color", id_color_rand);
		envir_shader->UnuseShader();
	};

	envir_frameBuffer->UnbindFrameBuffer();	
	delete indexArray;
}

Environment::Environment()
{

}

Environment::~Environment()
{
	envir_shader->DelShad();
	envir_IBL_spec.DelTexture();
	envir_frameBuffer->Del();
}

void Environment::ChangeEnvirTexture(const std::string& texpath) const
{

}

void Environment::ChangeEnvirType(EnvironmentType type) const
{

}

void Environment::BindFrameBuffer() const
{
	envir_frameBuffer->BindFrameBuffer();
}

void Environment::UnBindFrameBuffer() const
{
	envir_frameBuffer->UnbindFrameBuffer();
}

void Environment::SwapFrameBuffer(FBType type)
{
	envir_shader->UseShader();
	envir_shader->SetValue("screen_texture", BUFFER_TEXTURE + type);
}

void Environment::GenFloatData() const
{

}

void Environment::RenderEnvironment(Camera* cam, int act)
{
	o_vertArry.Bind();
	envir_shader->UseShader();
	o_indexBuffer.Bind();
	envir_frameBuffer->BindFrameBufferTex(4, COMBINE_FB, ID_FB, RAND_FB, ALPHA_FB);
	envir_IBL_spec.Bind();

	if (envir_shader->is_shader_changed)
		envir_shader->InitShader();

	//DEBUG(envir_shader->is_shader_changed)

	if(cam->is_invUniform_changed || envir_shader->is_shader_changed)
		envir_shader->SetValue("cam_rotM", cam->o_rotMat);

	if(act != 0)
		envir_shader->SetValue("activeID", (float)act);

	if (cam->is_frustum_changed || envir_shader->is_shader_changed) {
		envir_shader->SetValue("cam_fov", glm::radians(cam->cam_pers));
		envir_shader->SetValue("cam_ratio", cam->cam_w / cam->cam_h);
	}

	envir_shader->SetValue("U_gamma", envir_gamma);
	envir_shader->is_shader_changed = false;

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
