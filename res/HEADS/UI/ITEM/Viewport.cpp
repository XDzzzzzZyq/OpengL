#include "Viewport.h"

UI::Viewport::Viewport()
{

}

UI::Viewport::~Viewport()
{

}

void UI::Viewport::ResetUV(const ImVec2& min, const ImVec2& max)
{
	uv_min = min;
	uv_max = max;
}

void UI::Viewport::ResetBufferID(GLuint id)
{
	vp_fboTex_ID = id;
}

void UI::Viewport::RenderItem() const
{
	ImGui::GetWindowDrawList()->AddImage(

		(ImTextureID)vp_fboTex_ID,
		ImGui::GetWindowPos(),
		ImGui::GetWindowPos() + vp_size,
		ImVec2(0, 1),
		ImVec2(1, 0)

	);
}

UI::Viewport::Viewport(GLuint texID, const ImVec2& vp_size)
	:vp_fboTex_ID(texID), vp_size(vp_size)
{
	uitm_name = "Viewport";
	uitm_type = IMAGE_OUTP;
}

UI::Viewport::Viewport(GLuint texID)
	: vp_fboTex_ID(texID)
{
	uitm_name = "Viewport";
	uitm_type = IMAGE_OUTP;
}
