#pragma once
#include "ImguiItem.h"

namespace UI {

	class TextureViewer : public ImguiItem
	{
	private:
		GLuint vp_fboTex_ID = 0;
	public:
		ImVec2 uv_min;
		ImVec2 uv_max;
		ImVec2 vp_size;

		TextureViewer();
		TextureViewer(const std::string& name, GLuint texID);
		TextureViewer(const std::string& name, GLuint texID, const ImVec2& vp_size);
		~TextureViewer();

		bool show_size;

		void ResetUV(const ImVec2& min, const ImVec2& max) override;
		void ResetSize(const ImVec2& size) override;
		void ResetBufferID(GLuint id) override;
		void RenderItem() const override;
	};
}


