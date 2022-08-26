#include "OpaButton.h"

UI::OpaButton::OpaButton()
{
	uitm_type = SELECT_INP;
}

UI::OpaButton::OpaButton(const std::string& name, int id)
{
	uitm_name = name;
	objectID = id;
}

UI::OpaButton::~OpaButton()
{
	uitm_type = SELECT_INP;
}

void UI::OpaButton::RenderItem() const
{
	if (is_activated)
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1, 0.5, 0, 1);
		is_button_pressed = ImGui::Selectable(uitm_name.c_str(), &is_button_pressed, 0, bt_size);
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
	}
	else
	{
		is_button_pressed = ImGui::Selectable(uitm_name.c_str(), &is_button_pressed, 0, bt_size);
	}

	if (is_button_pressed)
		is_activated = !is_activated;
}

void UI::OpaButton::ResetSize(const ImVec2& size)
{
	bt_size = size;
}
