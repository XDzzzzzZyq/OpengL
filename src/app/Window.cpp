#include "Window.h"

Window::Window(int width, int height, const std::string& title)
{
	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");

	glfw_initialized = true;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!handle)
	{
		glfwTerminate();
		glfw_initialized = false;
		throw std::runtime_error("glfwCreateWindow failed");
	}

	glfwMakeContextCurrent(handle);
}

Window::~Window()
{
	if (handle)
		glfwDestroyWindow(handle);
	if (glfw_initialized)
		glfwTerminate();
}
