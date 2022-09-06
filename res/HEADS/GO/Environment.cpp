#include "Environment.h"

Environment::Environment(const std::string& texpath)
{

	o_type = GO_ENVIR;
	envir_shader = Shaders("IBRShader");	

	envir_hdr = Texture(texpath, HDR_TEXTURE, GL_REPEAT);
	envir_hdr.Bind(); 
	//envir_hdr.Tex_slot = envir_hdr.Tex_type;

	envir_frameBuffer = FrameBuffer(4, COMBINE_FB, ID_FB, RAND_FB, ALPHA_FB);	

	o_name = "Environment." + std::to_string(GetObjectID());

	o_vertBuffer = VertexBuffer(screenQuad.data(), screenQuad.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(2); //2D position
	layout.Push<float>(2); //UV

	o_vertArry.AddBuffer(o_vertBuffer, layout);

	std::vector<GLuint>* indexArray = new std::vector<GLuint>{ 0,2,1,1,2,3 };
	GLuint* index = indexArray->data();

	o_indexBuffer = IndexBuffer(index, indexArray->size() * sizeof(GLuint)); 

	envir_shader->UseShader();

	envir_shader->SetValue("hdr_texture",         HDR_TEXTURE);
	envir_shader->SetValue("screen_texture",      BUFFER_TEXTURE + COMBINE_FB);
	envir_shader->SetValue("id_texture",          BUFFER_TEXTURE + ID_FB);
	envir_shader->SetValue("select_texture",		 BUFFER_TEXTURE + ALPHA_FB);
	envir_shader->SetValue("ID_color",            id_color);
	envir_shader->SetValue("RAND_color",          id_color_rand);

	envir_shader->UnuseShader();
	//frame_buffer.Unbind();

	envir_frameBuffer->UnbindFrameBuffer();	
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

Environment::Environment()
{

}

Environment::~Environment()
{
	envir_shader->DelShad();
	envir_hdr.DelTexture();
	envir_frameBuffer->Del();
}

void Environment::ChangeEnvirTexture(const std::string& texpath) const
{

}

void Environment::ChangeEnvirType(const EnvironmentType& type) const
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
	envir_hdr.Bind();
	if(cam->is_invUniform_changed)
		envir_shader->SetValue("cam_rotM", cam->o_rotMat);

	if(act != -1)
		envir_shader->SetValue("activeID", (float)act);

	if (cam->is_frustum_changed) {
		envir_shader->SetValue("cam_fov", glm::radians(cam->cam_pers));
		envir_shader->SetValue("cam_ratio", cam->cam_w / cam->cam_h);
	}

	envir_shader->SetValue("U_gamma", envir_gamma);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
