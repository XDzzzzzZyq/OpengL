#include "pch.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

TEST(GTestBasic, Tests) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(GLEnvir, Tests) {
	EXPECT_TRUE(glfwInit()) << "glfw init error" << std::endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(100, 100, "TEST_WINDOW", NULL, NULL);
	EXPECT_TRUE(window) << "window error" << std::endl;
	glfwMakeContextCurrent(window);

	EXPECT_EQ(glewInit(), GLEW_OK) << "glew error" << std::endl;
	EXPECT_EQ(glGetError(), GL_NO_ERROR) << "OpenGL Error: " << glGetError() << std::endl;
}