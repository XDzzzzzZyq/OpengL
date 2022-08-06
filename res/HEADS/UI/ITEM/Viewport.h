#pragma once
#include "ImguiItem.h"

namespace UI {

	class Viewport : public ImguiItem
	{
	private:
		GLuint vp_fboTex_ID = 0;
	public:
		ImVec2 uv_min;
		ImVec2 uv_max;
		ImVec2 vp_size;

		Viewport();
		Viewport(GLuint texID);
		Viewport(GLuint texID, const ImVec2& vp_size);
		~Viewport();

		void ResetUV(const ImVec2& min, const ImVec2& max);
		void ResetBufferID(GLuint id);
		void RenderItem() const override;
	};
}


