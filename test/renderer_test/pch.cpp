//
// pch.cpp
//

#include "pch.h"

void RendererEnvir::SetUpTestSuite()
{
}

void RendererEnvir::TearDownTestSuite()
{

}

void RendererEnvir::SetUp()
{
	EXPECT_TRUE(glfwInit()) << "glfw init error" << std::endl;

	GLFWwindow* window = glfwCreateWindow(100, 100, "TEST_WINDOW", NULL, NULL);
	EXPECT_TRUE(window) << "window error" << std::endl;
	glfwMakeContextCurrent(window);

	EXPECT_EQ(glewInit(), GLEW_OK) << "glew error" << std::endl;
	EXPECT_EQ(glGetError(), GL_NO_ERROR) << "OpenGL Error: " << glGetError() << std::endl;

	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << version << "\n";
	gl_version = (float)std::atof((const char*)version);

	if (gl_version < 4.0)
		return;

	GLint max_resolution_w, max_resolution_h;
	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &max_resolution_w);
	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &max_resolution_h);

	std::cout << "fb_size = [" << max_resolution_w << ":" << max_resolution_h << "]\n";
}

#include "Texture.h"
#include "Shaders.h"
void RendererEnvir::TearDown()
{
	TextureLib::ResetTexLib();
	ComputeShader::ResetComputeLib();
	glfwTerminate();
}
