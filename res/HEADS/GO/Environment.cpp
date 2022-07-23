#include "Environment.h"

Environment::Environment(const std::string& texpath, GLuint slot)
{
	//envir_HDR = Texture(texpath, IMAGE_TEXTURE, GL_REPEAT);
	//
	
	//
	envir_shader = Shaders("res/shaders/IBRShader.shader");

	envir_HDR = Texture("", NONE_TEXTURE, GL_NEAREST);
	envir_HDR.Bind(slot);
	envir_HDR.Tex_slot = slot;

	glGenRenderbuffers(1, &renderBuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_W, SCREEN_H);

	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, envir_HDR.GetTexID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer_id);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUG("framebuffer error")
	}
	else
	{
		std::cout<<"framebuffer is complete\n";
	}
	
	o_name = "Environment." + std::to_string(GetObjectID());

	o_vertBuffer = VertexBuffer(screenQuad.data(), screenQuad.size() * sizeof(float));

	BufferLayout layout;
	layout.Push<float>(2); //2D position
	layout.Push<float>(2); //UV

	o_vertArry.AddBuffer(o_vertBuffer, layout);

	envir_shader.UnuseShader();
	envir_HDR.Unbind();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
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
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);
}

void Environment::UnBindFrameBuffer() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Environment::GenFloatData() const
{

}

void Environment::RenderEnvironment() const
{
	envir_shader.UseShader();
	
	glBindVertexArray(o_vertArry.GetVertArrayID());

	envir_HDR.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}
