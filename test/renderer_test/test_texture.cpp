#include "pch.h"
#include "macros.h"

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
		auto tex2 = Texture("testImg.png", PNG_TEXTURE, GL_REPEAT);
		EXPECT_TRUE(tex2.GetTexID() != 0);
		std::cout << tex2.GetTexID() << " : " << tex2.GetTexName() << "\n";
		GLERRTEST;
	}
}

TEST_F(RendererEnvir, CubeMap) {
	{
		auto tex = Texture("hdr/room.hdr", HDR_TEXTURE, GL_MIRRORED_REPEAT);
		EXPECT_TRUE(tex.GetTexID() != 0);
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
		//GLERRTEST;
		EXPECT_EQ(cube.GetW(), 512);

		auto rec = Texture();
		rec.GenERectMapFrom(cube, w, h);
		std::cout << rec.GetTexID() << " : " << rec.GetTexName() << "\n";
		//GLERRTEST;

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

