#include "Button.h"

UI::Button::Button(const std::string& name)
{
	notagname = uitm_name = name;
	uitm_type = BUTTON_INP;
}

UI::Button::~Button()
{
	uitm_type = BUTTON_INP;
}

void UI::Button::RenderItem()const
{
	if (ImGui::Button(GetCharName()))
		if (ButtonFunc)
			ButtonFunc();
}
