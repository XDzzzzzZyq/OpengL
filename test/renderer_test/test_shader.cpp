#include "pch.h"
#include "macros.h"

#include "shaders/RenderShader.h"
#include "shaders/ComputeShader.h"
#include "Texture.h"
TEST_F(RendererEnvir, RenderShader) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	RenderShader shader = RenderShader("Empty", "Empty");
	GLERRTEST;

	int v_id = shader.GetShaderID(VERTEX_SHADER);
	int f_id = shader.GetShaderID(FRAGMENT_SHADER);
	EXPECT_NE(v_id, f_id);

	auto& sh_struct = shader.GetShaderUnit(VERTEX_SHADER)->sh_struct;
	Args args = { {FLOAT_PARA,"a"}, {FLOAT_PARA,"b"} };
	sh_struct->DefFunc(FLOAT_PARA, "testFunc", "return a + b;", args);
	EXPECT_TRUE(sh_struct->is_struct_changed);

	shader.GenerateShader(VERTEX_SHADER);
	EXPECT_TRUE(shader.is_shader_changed);
	EXPECT_NE(shader.GetShaderUnit(VERTEX_SHADER)->sh_code.find("testFunc"), std::string::npos);

	shader.RelinkShader(VERTEX_SHADER);
	GLERRTEST;

	int v_id_new = shader.GetShaderID(VERTEX_SHADER);
	EXPECT_NE(v_id_new, v_id);	// Expect a new shader ID after relinking
	EXPECT_NE(v_id_new, 0);

	sh_struct->DefFunc(FLOAT_PARA, "testFunc_wrong", "return a + b 123123", args); // Wrong function
	EXPECT_TRUE(sh_struct->is_struct_changed);

	shader.GenerateShader(VERTEX_SHADER);
	EXPECT_TRUE(shader.is_shader_changed);
	EXPECT_NE(shader.GetShaderUnit(VERTEX_SHADER)->sh_code.find("testFunc_wrong"), std::string::npos);

	shader.RelinkShader(VERTEX_SHADER);
	GLERRTEST;

	int v_id_new_new = shader.GetShaderID(VERTEX_SHADER);
	EXPECT_EQ(v_id_new, v_id_new_new);	// Expect new shader ID should keep the same
}

TEST_F(RendererEnvir, RenderShader_MixedExtensions) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	// Test that shader loading works with mixed extension usage
	// This should work: one with extension, one without
	RenderShader shader = RenderShader("Empty", "Empty.frag");
	GLERRTEST;

	int v_id = shader.GetShaderID(VERTEX_SHADER);
	int f_id = shader.GetShaderID(FRAGMENT_SHADER);
	EXPECT_NE(v_id, f_id);
	EXPECT_NE(v_id, -1);
	EXPECT_NE(f_id, -1);
}

glm::vec4 SAT(const std::vector<glm::vec4>& d, int index, int width = 4) {
	int x = index % width;
	int y = index / width;
	glm::vec4 res = glm::vec4(0.0);

	LOOP(x + 1) {
		LOOP_N(y + 1, j) {
			res += d[i + j * width];
		}
	}

	return res;
}

glm::vec4 Box_Blur(const std::vector<glm::vec4>& d, int index, int r, int width = 4) {
	int x = index % width;
	int y = index / width;
	glm::vec4 res = glm::vec4(0.0);

	int n = 0;
	LOOP(2 * r + 1) {
		LOOP_N(2 * r + 1, j) {
			int loc_x = x + i - r;
			int loc_y = y + j - r;
			if (loc_x < 0 || loc_x >= width || loc_y < 0 || loc_y >= width)
				continue;
			res += d[loc_x + loc_y * width];
			n += 1;
		}
	}
	res /= float(n);
	return res;
}

#include "operator.h"
TEST_F(RendererEnvir, ComputeShader) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	GLint maxSharedMem = 0;
	glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &maxSharedMem);
	std::cout << "Max compute shared memory size: " << maxSharedMem << " bytes" << std::endl;

	auto& sat = ComputeShader::ImportShader("convert/SAT");
	EXPECT_TRUE(sat.GetShaderID(COMPUTE_SHADER) != 0);
	GLERRTEST;

	{
		const int w = 4;
		auto tex = TextureLib::Noise_2D_4x4();
		EXPECT_TRUE(tex->GetTexID() != 0);
		std::cout << tex->GetTexID() << " : " << tex->GetTexName() << "\n";
		GLERRTEST;

		std::vector<glm::vec4> data(w * w, glm::vec4(-1));
		tex->Bind();
		auto [_, layout, type, gl_type] = Texture::ParseFormat(tex->tex_type);
		glGetTexImage(gl_type, 0, layout, type, data.data());
		GLERRTEST;

		Texture tex_temp = Texture(tex->GetW(), tex->GetH(), IBL_TEXTURE);

		/* SAT construction */

		tex->BindC(0, GL_READ_ONLY);
		tex_temp.BindC(1, GL_WRITE_ONLY);
		sat.RunComputeShader({ w,1 });

		tex_temp.BindC(0, GL_READ_ONLY);
		tex->BindC(1, GL_WRITE_ONLY);
		sat.RunComputeShader({ w,1 });

		{
			DEBUG(" >>> SAT construction");
			tex->Bind();
			std::vector<glm::vec4> satdata(w * w, glm::vec4(-1));
			glGetTexImage(gl_type, 0, layout, type, satdata.data());
			GLERRTEST;

			LOOP(w * w) {
				glm::vec4 s = SAT(data, i);
				EXPECT_GE(1e-5, glm::distance(satdata[i], s)) << " at (" << i / w << "," << i % w << ")\n";
			}
		}

		/* Box Blur with radius 0 */

		auto& blur = ComputeShader::ImportShader("pps/Box_Blur");
		GLERRTEST;

		{
			DEBUG(" >>> Box Blur with radius 0");
			Texture tex_rec = Texture(tex->GetW(), tex->GetH(), IBL_TEXTURE);
			tex->BindC(0, GL_READ_ONLY);
			tex_rec.BindC(1, GL_WRITE_ONLY);
			blur.UseShader();
			blur.SetValue("r", 0);
			blur.RunComputeShader({ w, w });
			GLERRTEST;

			tex_rec.Bind();
			std::vector<glm::vec4> blurdata(w * w, glm::vec4(-1));
			glGetTexImage(gl_type, 0, layout, type, blurdata.data());
			GLERRTEST;

			LOOP(w * w) {
				glm::vec4 s = data[i];
				EXPECT_GE(1e-5, glm::distance(blurdata[i], s)) << " at (" << i / w << "," << i % w << ")\n";
			}
		}

		/* Box Blur with radius 1 */

		{
			DEBUG(" >>> Box Blur with radius 1");
			Texture tex_rec = Texture(tex->GetW(), tex->GetH(), IBL_TEXTURE);
			tex->BindC(0, GL_READ_ONLY);
			tex_rec.BindC(1, GL_WRITE_ONLY);
			blur.UseShader();
			blur.SetValue("r", 1);
			blur.RunComputeShader({ w, w });
			GLERRTEST;

			tex_rec.Bind();
			std::vector<glm::vec4> blurdata(w * w, glm::vec4(-1));
			glGetTexImage(gl_type, 0, layout, type, blurdata.data());
			GLERRTEST;

			LOOP(w * w) {
				glm::vec4 s = Box_Blur(data, i, 1);
				EXPECT_GE(1e-5, glm::distance(blurdata[i], s)) << " at (" << i / w << "," << i % w << ")\n";
			}
		}
	}
}

TEST_F(RendererEnvir, ComputeShader_SAT) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	auto tex = Texture("hdr/room.hdr", HDR_TEXTURE, GL_MIRRORED_REPEAT);
	EXPECT_TRUE(tex.GetTexID() != 0);
	std::cout << tex.GetTexID() << " : " << tex.GetTexName() << "\n";

	const int w = 512;
	auto cube = Texture();
	cube.GenCubeMapFrom(tex, w);
	cube.SaveTexture("cube", false);
	
	auto& sat = ComputeShader::ImportShader("convert/SAT_Cube");
	EXPECT_TRUE(sat.GetShaderID(COMPUTE_SHADER) != 0);
	GLERRTEST;
	{
		Texture cube_temp = Texture(w, w, IBL_CUBE_TEXTURE);
		GLERRTEST;
		/* SAT construction */

		cube.BindC(0, GL_READ_ONLY); GLERRTEST;
		cube_temp.BindC(1, GL_WRITE_ONLY); GLERRTEST;
		sat.RunComputeShader({ w,6 }); GLERRTEST;

		cube_temp.BindC(0, GL_READ_ONLY); GLERRTEST;
		cube.BindC(1, GL_WRITE_ONLY); GLERRTEST;
		sat.RunComputeShader({ w,6 }); GLERRTEST;

		cube.SaveTexture("cube_SAT", false);
		GLERRTEST;
	}
}