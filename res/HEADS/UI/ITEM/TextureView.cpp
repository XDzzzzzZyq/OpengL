#include "TextureView.h"

UI::TextureView::TextureView()
{

}

UI::TextureView::~TextureView()
{

}

void UI::TextureView::ResetUV(const ImVec2& min, const ImVec2& max)
{
	uv_min = min;
	uv_max = max;
}

void UI::TextureView::ResetSize(const ImVec2& size)
{
	vp_size = size;
}

void UI::TextureView::ResetBufferID(GLuint id)
{
	vp_fboTex_ID = id;
}

void UI::TextureView::RenderItem() const
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

UI::TextureView::TextureView(const std::string& name, GLuint texID, const ImVec2& vp_size)
	:vp_fboTex_ID(texID), vp_size(vp_size)
{
	uitm_name = name;
	uitm_type = IMAGE_OUTP;
}

UI::TextureView::TextureView(const std::string& name, GLuint texID)
	: vp_fboTex_ID(texID)
{
	uitm_name = name;
	uitm_type = IMAGE_OUTP;
}
