#include "OpaButton.h"

UI::OpaButton::OpaButton()
{

}

UI::OpaButton::~OpaButton()
{

}

void UI::OpaButton::RenderItem() const
{
	is_button_pressed = ImGui::Selectable(uitm_name.c_str(), &is_button_pressed, 0, bt_size);
}

void UI::OpaButton::ResetSize(const ImVec2& size)
{
	bt_size = size;
}

UI::OpaButton::OpaButton(const std::string& name)
{
	uitm_name = name;
}
