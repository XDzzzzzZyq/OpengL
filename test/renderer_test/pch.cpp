//
// pch.cpp
//

#include "pch.h"

#include <filesystem>

#include "Texture.h"
#include "shaders/RenderShader.h"
#include "shaders/ComputeShader.h"

void RendererEnvir::SetUpTestSuite()
{
	const auto result_root = std::filesystem::path("result");
	std::filesystem::remove_all(result_root);
	std::filesystem::create_directories(result_root);
	std::filesystem::create_directories(result_root / "room_cube_faces");
	std::filesystem::create_directories(result_root / "depth_cube_faces");
}

void RendererEnvir::TearDownTestSuite()
{

}

void RendererEnvir::SetUp()
{

	EXPECT_TRUE(glfwInit()) << "glfw init error" << std::endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	Shaders::folder_root = shader_root;
	TextureLib::root_dir = tex_root;
}
void RendererEnvir::TearDown()
{
	TextureLib::ResetTexLib();
	ComputeShader::ResetComputeLib();
	glfwTerminate();
}
