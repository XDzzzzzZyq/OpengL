#include "ImguiMButton.h"

UI::ImguiMButton::ImguiMButton()
{

}

UI::ImguiMButton::ImguiMButton(std::string _name, std::string _shortcut)
	:ImguiMenuItem(_name, _shortcut, MenuItemType::BUTTON_MITEM)
{
	//assert(!_shortcut.empty()); no shortcut is available

	mitem_func = [this](bool) {std::cout << mitem_shortcut << "\n"; return true; };
}

void UI::ImguiMButton::RenderMenuItem()
{
	if (ImGui::MenuItem(
		mitem_name.c_str(),
		mitem_shortcut.c_str(),
		false,
		mitem_enable
	)) {
		if(mitem_func)
			mitem_func(true);
	}
}
