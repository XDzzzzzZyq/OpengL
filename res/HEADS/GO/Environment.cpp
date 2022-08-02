#include "Environment.h"

Environment::Environment(const std::string& texpath)
{
	//envir_HDR = Texture(texpath, IMAGE_TEXTURE, GL_REPEAT);
	//
	
	//
	envir_shader = Shaders("res/shaders/IBRShader.shader");	

	envir_hdr = Texture(texpath, HDR_TEXTURE, GL_REPEAT);
	envir_hdr.Bind(envir_hdr.Tex_type); 
	//envir_hdr.Tex_slot = envir_hdr.Tex_type;

	envir_frameBuffer = FrameBuffer();	

	
	o_name = "Environment." + std::to_string(GetObjectID());

	o_vertBuffer = VertexBuffer(screenQuad.data(), screenQuad.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(2); //2D position
	layout.Push<float>(2); //UV

	o_vertArry.AddBuffer(o_vertBuffer, layout);

	std::vector<GLuint>* indexArray = new std::vector<GLuint>{ 0,2,1,1,2,3 };
	GLuint* index = indexArray->data();

	o_indexBuffer = IndexBuffer(index, indexArray->size() * sizeof(GLuint)); 

	envir_shader.UseShader();

	envir_shader.SetValue("hdr_texture", HDR_TEXTURE);
	envir_shader.SetValue("screen_texture", BUFFER_TEXTURE);

	envir_shader.UnuseShader();
	//frame_buffer.Unbind();

	envir_frameBuffer->UnbindFrameBuffer();	
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

Environment::Environment()
{

}

Environment::~Environment()
{

}

void Environment::ChangeEnvirTexture(const std::string& texpath) const
{

}

void Environment::ChangeEnvirType(const EnvironmentType& type) const
{

}

void Environment::BindFrameBuffer() const
{
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);
	envir_frameBuffer->BindFrameBuffer();
}

void Environment::UnBindFrameBuffer() const
{
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	envir_frameBuffer->UnbindFrameBuffer();
}

void Environment::GenFloatData() const
{

}

void Environment::RenderEnvironment(const Camera& cam)
{
	o_vertArry.Bind();
	envir_shader.UseShader();
	o_indexBuffer.Bind();
	//frame_buffer.Bind(frame_buffer.Tex_type);
	envir_frameBuffer->BindFrameBufferTex();
	envir_hdr.Bind(envir_hdr.Tex_type);

	envir_shader.SetValue("cam_rotM", cam.o_rotMat);
	envir_shader.SetValue("cam_fov", glm::radians(cam.cam_pers));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	
	o_indexBuffer.Unbind();
	envir_hdr.Unbind();
	envir_shader.UnuseShader();
	//frame_buffer.Unbind();
	envir_frameBuffer->UnbindFrameBufferTex();
	o_vertArry.Unbind();
}
