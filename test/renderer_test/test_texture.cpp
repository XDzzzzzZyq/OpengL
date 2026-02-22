#include "pch.h"
#include "macros.h"

#include "Texture.h"
TEST_F(RendererEnvir, Texture) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	{
		auto tex = TextureLib::Noise_2D_4x4xN(5);
		EXPECT_NE(tex->GetTexID(), 0);
		std::cout << tex->GetTexID() << " : " << tex->GetTexName() << "\n";
		GLERRTEST;
	}
	{
		auto tex = TextureLib::Noise_2D_4x4();
		EXPECT_NE(tex->GetTexID(), 0);
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
		auto tex2 = Texture("testImg.png", Texture::RGBA_TEXTURE, GL_REPEAT);
		EXPECT_NE(tex2.GetTexID(), 0);
		std::cout << tex2.GetTexID() << " : " << tex2.GetTexName() << "\n";
		GLERRTEST;
	}
}

TEST_F(RendererEnvir, Texture_CubeMap) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	{
		auto tex = Texture("hdr/room.hdr", Texture::HDR_TEXTURE, GL_MIRRORED_REPEAT);
		EXPECT_NE(tex.GetTexID(), 0);
		std::cout << tex.GetTexID() << " : " << tex.GetTexName() << "\n";
		GLERRTEST;

		int w = tex.GetW();
		int h = tex.GetH();

		std::vector<glm::vec4> data(w * h, glm::vec4(-1));
		tex.Bind();
		auto [_, layout, type, gl_type] = Texture::ParseFormat(tex.tex_type);
		glGetTexImage(gl_type, 0, layout, type, data.data());
		GLERRTEST;

		auto cube = Texture();
		cube.GenCubeMapFrom(tex, 512);
		std::cout << tex.GetTexID() << " : " << tex.GetTexName() << "\n";
		GLERRTEST;
		EXPECT_EQ(cube.GetW(), 512);

		auto rec = Texture();
		rec.GenERectMapFrom(cube, w, h);
		std::cout << rec.GetTexID() << " : " << rec.GetTexName() << "\n";
		GLERRTEST;

		EXPECT_EQ(w, rec.GetW());
		EXPECT_EQ(h, rec.GetH());
		EXPECT_TRUE(tex.tex_type == rec.tex_type);

		std::vector<glm::vec4> data_rec(w * h, glm::vec4(-1));
		rec.Bind();
		auto [_2, layout2, type2, gl_type2] = Texture::ParseFormat(rec.tex_type);
		glGetTexImage(gl_type2, 0, layout2, type2, data_rec.data());

		float err = 0.0f;
		float max = 0.0f;
		LOOP(w * h) {
			float d = glm::distance(data_rec[i], data[i]);
			//EXPECT_GE(1.5, d) << " at (" << i / w << "," << i % w << ")\n";
			max = std::max(max, d);
			err += d;
		}
		err /= (w * h);
		DEBUG(max);
		DEBUG(err);

		EXPECT_GE(1.5, max);
		EXPECT_GE(0.004, err);
	}
}

Texture get_texture(const std::string& path, Texture::TextureType type) {
	return Texture(path, type, GL_REPEAT);
}

TEST_F(RendererEnvir, Texture_RAII) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	int temp_id = 0;
	Texture tex{};
	EXPECT_EQ(tex.GetTexID(), 0);
	{
		auto temp = Texture("avatar1.png", Texture::RGBA_TEXTURE, GL_REPEAT);
		temp_id = temp.GetTexID();
		EXPECT_EQ(temp_id, 1);
		GLERRTEST;

		tex = temp;
		GLERRTEST;
	}
	GLERRTEST;
	EXPECT_EQ(tex.GetTexID(), 2);
	EXPECT_EQ(tex.tex_type, Texture::RGBA_TEXTURE);

	{
		auto temp = get_texture("avatar1.png", Texture::RGBA_TEXTURE);
		temp_id = temp.GetTexID();
		EXPECT_EQ(temp_id, 1);
		GLERRTEST;

		tex = temp;
		GLERRTEST;
	}
	GLERRTEST;
	EXPECT_EQ(tex.GetTexID(), 2);
	EXPECT_EQ(tex.tex_type, Texture::RGBA_TEXTURE);

	{
		auto temp = Texture("hdr/room.hdr", Texture::HDR_TEXTURE, GL_MIRRORED_REPEAT);
		temp_id = temp.GetTexID();
		EXPECT_EQ(temp_id, 1);
		GLERRTEST;

		tex = temp;
		GLERRTEST;
	}
	GLERRTEST;
	EXPECT_EQ(tex.GetTexID(), 2);
	EXPECT_EQ(tex.tex_type, Texture::HDR_TEXTURE);

	Texture tex2 = tex;
	GLERRTEST;
	tex2.SaveTexture("copy_test", false, false);
	GLERRTEST;
	EXPECT_EQ(tex.GetTexID(),  2);
	EXPECT_EQ(tex2.GetTexID(), 1);
}

#include <filesystem>
TEST_F(RendererEnvir, Texture_Save) {
	if (gl_version < 4.0)
		GTEST_SKIP();

	{
		auto tex = Texture("hdr/room.hdr", Texture::HDR_TEXTURE, GL_MIRRORED_REPEAT);
		EXPECT_NE(tex.GetTexID(), 0);
		std::cout << tex.GetTexID() << " : " << tex.GetTexName() << "\n";
		GLERRTEST;

		int w = tex.GetW();
		int h = tex.GetH();

		tex.SaveTexture("room_save", false, false);
		GLERRTEST;
		EXPECT_TRUE(std::filesystem::exists("result/room_save.hdr"));

		tex.SaveTexture("room_save", true, false);
		GLERRTEST;
		EXPECT_TRUE(std::filesystem::exists("result/room_save.png"));

		auto cube = Texture(); GLERRTEST;
		cube.GenCubeMapFrom(tex, 512); GLERRTEST;
		std::cout << tex.GetTexID() << " : " << tex.GetTexName() << "\n";
		GLERRTEST;
		EXPECT_EQ(cube.GetW(), 512);

		// TODO: HDRCube -> PNGCube
		//cube.SaveTexture("room_cube", true, true);
		//GLERRTEST;
		//EXPECT_TRUE(std::filesystem::exists("result/room_cube.hdr"));

		cube.SaveTexture("room_cube_faces", false, true);
		GLERRTEST;
		LOOP(6) {
			std::string outputPath = "result/room_cube_faces/room_cube_faces_" + std::to_string(i + 1) + ".hdr";
			EXPECT_TRUE(std::filesystem::exists(outputPath));
		}
	}
	{
		auto tex = Texture("avatar1.png", Texture::RGBA_TEXTURE, GL_REPEAT);
		EXPECT_NE(tex.GetTexID(), 0);
		GLERRTEST;

		tex.SaveTexture("png_save");
		EXPECT_TRUE(std::filesystem::exists("result/png_save.png"));
	}
}

TEST_F(RendererEnvir, Depth_Texture_Save) {
	if (gl_version < 4.0)
		GTEST_SKIP();
	{
		auto depth2d = Texture(32, 32, Texture::DEPTH_TEXTURE);
		float depth_val = 0.5f;
		glClearTexImage(depth2d.GetTexID(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth_val);
		GLERRTEST;

		depth2d.SaveTexture("depth2d_hdr", false, false);
		GLERRTEST;
		EXPECT_TRUE(std::filesystem::exists("result/depth2d_hdr.hdr"));
		DEBUG("PASS");
		depth2d.SaveTexture("depth2d_png", true, false);
		GLERRTEST;
		EXPECT_TRUE(std::filesystem::exists("result/depth2d_png.png"));
	}
	{
		auto depth_cube = Texture(32, 32, Texture::DEPTH_CUBE_TEXTURE);
		float depth_val = 0.75f;
		glClearTexImage(depth_cube.GetTexID(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth_val);
		GLERRTEST;

		depth_cube.SaveTexture("depth_cube_hdr", false, false);
		GLERRTEST;
		EXPECT_TRUE(std::filesystem::exists("result/depth_cube_hdr.hdr"));

		depth_cube.SaveTexture("depth_cube_faces", false, true);
		GLERRTEST;
		LOOP(6) {
			std::string outputPath = "result/depth_cube_faces/depth_cube_faces_" + std::to_string(i + 1) + ".hdr";
			EXPECT_TRUE(std::filesystem::exists(outputPath));
		}

		depth_cube.SaveTexture("depth_cube_png", true, true);
		GLERRTEST;
		EXPECT_TRUE(std::filesystem::exists("result/depth_cube_png.png"));
	}
}

