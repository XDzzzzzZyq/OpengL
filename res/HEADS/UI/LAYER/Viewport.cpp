#include "Viewport.h"

Viewport::Viewport()
{
	uly_name = "";
}

Viewport::Viewport(const std::string& name)
	:Viewport(name, 0)
{}

Viewport::Viewport(const std::string& name, GLuint texID)
	: Viewport(name, 0, ImVec2(SCREEN_W, SCREEN_H))
{}

Viewport::Viewport(const std::string& name, GLuint texID, const ImVec2& vp_size)
{
	uly_name = name;
	PushItem<UI::TextureView>("Viewport", texID, vp_size);
}

Viewport::~Viewport()
{

}

void Viewport::UpdateLayer()

{
}

void Viewport::RenderLayer()
{
	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		item_list[0]->RenderItem();

		GetLayerSize();
		if(uly_name == "Viewport")
			EventListener::viewport_offset = ImGui::GetWindowPos() - window_pos;

		EventListener::viewport_offset = ImGui::GetWindowPos() - window_pos;
		EventListener::is_in_viewport = ITEM::is_inside(uly_size);

		if (IsResizingFin())
			if (resize_event) {
				item_list[0]->ResetSize(uly_size + ImVec2(10, 10));
				resize_event();
			}

		is_size_changed_b = is_size_changed;
		is_size_changed = false;
		ImGui::End();
	}
	else {
		EventListener::is_in_viewport = false;
		ImGui::End();
	}
}
