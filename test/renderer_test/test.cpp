#include "pch.h"
#include "macros.h"

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

#include "Texture.h"
#include "Shaders.h"
TEST_F(RendererEnvir, Texture) {
	{
		auto tex = TextureLib::Noise_2D_4x4xN(5);
		EXPECT_TRUE(tex->GetTexID() != 0);
		std::cout << tex->GetTexID() << " : " << tex->GetTexName() << "\n";
		GLERRTEST;
	}
	{
		auto tex = TextureLib::Noise_2D_4x4();
		EXPECT_TRUE(tex->GetTexID() != 0);
		std::cout << tex->GetTexID() << " : " << tex->GetTexName() << "\n";
		GLERRTEST;

		std::vector<glm::vec4> data(4 * 4, glm::vec4(-1));
		tex->Bind();
		auto [_, layout, type, gl_type] = Texture::ParseFormat(tex->tex_type);
		glGetTexImage(gl_type, 0, layout, type, data.data());
		GLERRTEST;

		glm::vec4 sum{ 0 };
		LOOP(4 * 4)
			sum += data[i];
		EXPECT_TRUE(sum != glm::vec4(-4 * 4));
	}
	{
		auto tex2 = Texture(tex_root + "testImg.png", PNG_TEXTURE, GL_REPEAT);
		EXPECT_TRUE(tex2.GetTexID() != 0);
		std::cout << tex2.GetTexID() << " : " << tex2.GetTexName() << "\n";
		GLERRTEST;
	}
}

glm::vec4 SAT(const std::vector<glm::vec4>& d, int index, int width = 4) {
	int x = index % width;
	int y = index / width;
	glm::vec4 res = glm::vec4(0.0);

	LOOP(x+1)
		LOOP_N(y+1, j)
		res += d[i + j * width];

	return res;
}

#include "operator.h"
TEST_F(RendererEnvir, ComputeShader) {
	if (gl_version < 4.0)
		GTEST_SKIP("Skipped");

	auto& sat = ComputeShader::ImportShader(shader_root + "SAT");
	EXPECT_TRUE(sat.GetShaderID(COMPUTE_SHADER) != 0);
	GLERRTEST;

	{
		auto tex = TextureLib::Noise_2D_4x4();
		EXPECT_TRUE(tex->GetTexID() != 0);
		std::cout << tex->GetTexID() << " : " << tex->GetTexName() << "\n";
		GLERRTEST;

		std::vector<glm::vec4> data(4 * 4, glm::vec4(-1));
		tex->Bind();
		auto [_, layout, type, gl_type] = Texture::ParseFormat(tex->tex_type);
		glGetTexImage(gl_type, 0, layout, type, data.data());
		GLERRTEST;
		
		tex->BindC(0);
		sat.RunComputeShader({ 4,1 });
		sat.RunComputeShader({ 4,1 });

		tex->Bind();
		std::vector<glm::vec4> satdata(4 * 4, glm::vec4(-1));
		glGetTexImage(gl_type, 0, layout, type, satdata.data());
		GLERRTEST;

		LOOP(4 * 4) {
			glm::vec4 s = SAT(data, i);
			EXPECT_TRUE(glm::distance(satdata[i], s) < 0.01) << " at (" << i / 4 << "," << i % 4 << ")\n";
		}
	}
}