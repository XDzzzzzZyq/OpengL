#include "pch.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

TEST(GTestBasic, Tests) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(GLEnvir, Tests) {
	EXPECT_TRUE(glfwInit()) << "glfw init error" << std::endl;

	GLFWwindow* window = glfwCreateWindow(100, 100, "TEST_WINDOW", NULL, NULL);
	EXPECT_TRUE(window) << "window error" << std::endl;
	glfwMakeContextCurrent(window);

	EXPECT_EQ(glewInit(), GLEW_OK) << "glew error" << std::endl;
	EXPECT_EQ(glGetError(), GL_NO_ERROR) << "OpenGL Error: " << glGetError() << std::endl;
}