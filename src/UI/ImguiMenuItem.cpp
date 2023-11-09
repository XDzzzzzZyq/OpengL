#include "ImguiMenuItem.h"
#include "macros.h"


ImguiMenuItem::ImguiMenuItem()
	: ImguiMenuItem("") {}

ImguiMenuItem::ImguiMenuItem(const std::string& name)
	: ImguiMenuItem(name, "") {}

ImguiMenuItem::ImguiMenuItem(const std::string& name, const std::string& shortcut)
	: ImguiMenuItem(name, shortcut, BUTTON_MITEM) {}

ImguiMenuItem::ImguiMenuItem(const std::string& name, MenuItemType _type)
	: ImguiMenuItem(name, "", _type) {}
ImguiMenuItem::ImguiMenuItem(const std::string& name, const std::string& shortcut, MenuItemType _type)
	: mitem_name(name), mitem_shortcut(shortcut), mitem_type(_type) {}
