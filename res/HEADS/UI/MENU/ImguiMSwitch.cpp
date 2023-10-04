#include "ImguiMSwitch.h"
#include "macros.h"

UI::ImguiMSwitch::ImguiMSwitch()
{

}

UI::ImguiMSwitch::ImguiMSwitch(std::string _name)
	:ImguiMenuItem(_name, MenuItemType::BOOL_MITEM)
{
	mitem_func = [_name](bool _){DEBUG(_name); return false; };
}

void UI::ImguiMSwitch::BindSwitch(bool* _switch)
{
	mitem_switch = std::shared_ptr<bool>(_switch);
}

void UI::ImguiMSwitch::RenderMenuItem()
{
	if (ImGui::MenuItem(
		mitem_name.c_str(),
		mitem_shortcut.c_str(),
		mitem_switch.get(),
		mitem_enable
	)) {
		mitem_func(*mitem_switch);
	}
}
