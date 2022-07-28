#include "Environment.h"

Environment::Environment(const std::string& texpath)
{
	//envir_HDR = Texture(texpath, IMAGE_TEXTURE, GL_REPEAT);
	//
	
	//
	envir_shader = Shaders("res/shaders/IBRShader.shader");

	envir_hdr = Texture(texpath, HDR_TEXTURE, GL_NEAREST);
	envir_hdr.Bind(envir_hdr.Tex_type);
	envir_hdr.Tex_slot = envir_hdr.Tex_type;

#if 0



	frame_buffer = Texture("", BUFFER_TEXTURE, GL_NEAREST);
	frame_buffer.Bind(frame_buffer.Tex_type);
	frame_buffer.Tex_slot = frame_buffer.Tex_type;

	glGenRenderbuffers(1, &renderBuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_W, SCREEN_H);

	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer.GetTexID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer_id);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUG("framebuffer error")
	}
	else
	{
		std::cout << "framebuffer is complete\n";
	}

#else

	envir_frameBuffer = FrameBuffer();

#endif

	
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

void Environment::RenderEnvironment(const glm::mat4& cam_rotM, float fov)
{
	o_vertArry.Bind();
	envir_shader.UseShader();
	o_indexBuffer.Bind();
	//frame_buffer.Bind(frame_buffer.Tex_type);
	envir_frameBuffer->BindFrameBufferTex();
	envir_hdr.Bind(envir_hdr.Tex_type);

	envir_shader.SetValue("cam_rotM", cam_rotM);
	envir_shader.SetValue("cam_fov", fov);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	
	o_indexBuffer.Unbind();
	envir_hdr.Unbind();
	envir_shader.UnuseShader();
	//frame_buffer.Unbind();
	envir_frameBuffer->UnbindFrameBufferTex();
	o_vertArry.Unbind();
}
