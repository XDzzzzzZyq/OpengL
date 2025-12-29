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

#include "Transform.h"
TEST(Transform, Transform2D) {
	{
		Transform2D t2d{};
		t2d.SetPos({ 10.0f, 5.0f });
		t2d.SetRot(45.0f);
		t2d.SetScale({ 2.0f, 3.0f });

		EXPECT_TRUE(t2d.is_TransF_changed);

		EXPECT_TRUE(t2d.ApplyTransform());

		EXPECT_TRUE(!t2d.is_TransF_changed);
		EXPECT_TRUE(t2d.is_invTransF_changed);

		EXPECT_TRUE(t2d.GetInvTransform());
		EXPECT_TRUE(!t2d.is_invTransF_changed);

		EXPECT_TRUE(t2d.o_Transform * t2d.o_InvTransform == glm::mat3(1.0f));
		EXPECT_TRUE(t2d.o_InvTransform * t2d.o_Transform == glm::mat3(1.0f));
	}
}

inline bool mat4_near(
	const glm::mat4& a,
	const glm::mat4& b,
	float eps = 1e-5f)
{
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			if (fabs(a[c][r] - b[c][r]) > eps)
				return false;
	return true;
}

TEST(Transform, Transform3D) {
	{
		/* Inverse Calculation */
		Transform3D t3d{};
		t3d.SetPos({ 10.0f, 5.0f, 15.0f });
		t3d.SetRot({ 45.0f, -20.0f, 15.0f });
		t3d.SetScale({ 2.0f, 3.0f, 4.0f });

		EXPECT_TRUE(t3d.is_TransF_changed);

		EXPECT_TRUE(t3d.ApplyTransform());

		EXPECT_TRUE(!t3d.is_TransF_changed);
		EXPECT_TRUE(t3d.is_invTransF_changed);

		EXPECT_TRUE(t3d.GetInvTransform());
		EXPECT_TRUE(!t3d.is_invTransF_changed);

		EXPECT_TRUE(mat4_near(t3d.o_Transform * t3d.o_InvTransform, glm::mat4(1.0f)));
		EXPECT_TRUE(mat4_near(t3d.o_InvTransform * t3d.o_Transform, glm::mat4(1.0f)));
	}
	{
		/* Set Transform */
		Transform3D t1{};
		const float dz = 10.0f, rx = 90.0f, sx = 2.0f, sy = 4.0f, sz = 8.0f;
		t1.SetPos({ 0.0f, 0.0f, dz });
		t1.SetRot({ rx, 0.0f, 0.0f });
		t1.SetScale({ sx, sy, sz });

		EXPECT_TRUE(t1.is_TransF_changed);
		EXPECT_TRUE(t1.ApplyTransform());

		Transform3D t2{};
		t2.SetTrans(t1.o_Transform);
		EXPECT_TRUE(!t2.is_TransF_changed);
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_position, t1.o_position, 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_rot, t1.o_rot, 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_scale, t1.o_scale, 1e-5f)));

		Transform3D t3{};
		t3.SetTrans(t1.o_Transform, false, true, true);
		EXPECT_TRUE(t3.is_TransF_changed);
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t3.o_position, glm::vec3(0.0f), 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t3.o_rot, t1.o_rot, 1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t3.o_scale, t1.o_scale, 1e-5f)));
	}
	{
		/* Parenting */
		Transform3D t1{};
		const float dz = 10.0f, rx = 90.0f, sx = 2.0f, sy = 4.0f, sz = 8.0f;
		t1.SetPos({ 0.0f, 0.0f, dz });
		t1.SetRot({ rx, 0.0f, 0.0f });
		t1.SetScale({ sx, sy, sz });

		EXPECT_TRUE(t1.is_TransF_changed);

		Transform3D t2{};
		t2.SetParent(&t1, true);
		EXPECT_TRUE(!t1.is_TransF_changed);
		EXPECT_TRUE(t2.is_TransF_changed);

		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_position, glm::vec3(0.0f, -dz / sy, 0.0f),      1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_rot,      glm::vec3(-rx, 0.0f, 0.0f),           1e-5f)));
		EXPECT_TRUE(glm::all(glm::epsilonEqual(t2.o_scale,    glm::vec3(1.0f/sx, 1.0f/sz, 1.0f/sy), 1e-5f)));

		t2.ApplyAllTransform();
		EXPECT_TRUE(mat4_near(t2.o_Transform, glm::mat4(1.0f)));
	}
}