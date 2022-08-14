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

void UI::Viewport::ResetSize(const ImVec2& size)
{
	vp_size = size;
}

void UI::Viewport::ResetBufferID(GLuint id)
{
	vp_fboTex_ID = id;
}

void UI::Viewport::RenderItem() const
{

	ImGui::GetWindowDrawList()->AddImage(

		(ImTextureID)vp_fboTex_ID,
		ImGui::GetWindowPos() + ImVec2(0,23),
		ImGui::GetWindowPos() + vp_size + ImVec2(0, 23),
		ImVec2(0, 1),
		ImVec2(1, 0)

	);
	ImGui::Text("[ %.1f , %.1f ]", vp_size.x, vp_size.y);
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
