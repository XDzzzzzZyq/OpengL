#include "pch.h"
#include "macros.h"

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

	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* name = glGetString(GL_VENDOR); 
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glewVersion = glewGetString(GLEW_VERSION);
	const char* glfwVersion = glfwGetVersionString();
	std::cout << version << "\n" << name << "\n" << renderer << "\n GLEW:" << glewVersion << "\n GLFW:" << glfwVersion << "\n";

	GLint max_resolution_w, max_resolution_h;
	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &max_resolution_w);
	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &max_resolution_h);

	std::cout << "fb_size = [" << max_resolution_w << ":" << max_resolution_h << "]\n";

	glfwTerminate();
}


#include "xdz_math.h"
#include "xdz_matrix.h"
TEST(Mathlib, Tests) {
	EXPECT_EQ(5.0f, xdzm::float_dist(-3, 4));
	EXPECT_EQ(5.0f, xdzm::float_dist( 3, 4));

	EXPECT_EQ( 1.0f, xdzm::map01_11(1.0f));
	EXPECT_EQ( 0.0f, xdzm::map01_11(0.5f));
	EXPECT_EQ(-1.0f, xdzm::map01_11(0.0f));

	EXPECT_EQ(glm::vec3(-1.0f), xdzm::map01_11(glm::vec3(0.0f)));
	EXPECT_EQ(glm::vec3( 1.0f), xdzm::map01_11(glm::vec3(1.0f)));

	const std::vector<float> v{ 1,2,3 };
	EXPECT_EQ(glm::vec3(1, 2, 3), xdzm::stdVec3_vec3(v));

	const auto rand = xdzm::rand3nhv(10);
	EXPECT_EQ(rand.size(), 10);
	LOOP(10)
		EXPECT_TRUE(1.0f - glm::length(rand[i]) < 2*glm::epsilon<float>());

	EXPECT_EQ(4.0f, xdzm::_matrix_sum<4>(xdzm::identityMatrix));
}
