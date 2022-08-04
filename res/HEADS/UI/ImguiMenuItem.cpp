#include "ImguiMenuItem.h"

void ImguiMenuItem::ListenEvent()
{
	if (subm_press) {
		subm_func();
		subm_press = false;
	}
}

ImguiMenuItem::ImguiMenuItem()
{
	subm_name = "";
	subm_shortcut = "";
}

ImguiMenuItem::ImguiMenuItem(const std::string& name)
{
	subm_name = name;
	subm_shortcut = "";
}

ImguiMenuItem::ImguiMenuItem(const std::string& name, const std::string& shortcut)
{
	subm_name = name;
	subm_shortcut = shortcut;
}

ImguiMenuItem::~ImguiMenuItem()
{

}
