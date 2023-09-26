#include "ImguiMenuItem.h"
#include "macros.h"

void ImguiMenuItem::ListenEvent()
{
	if (mitem_press) {
		mitem_func();
		mitem_press = false;
		DEBUG(mitem_name)
	}
}

ImguiMenuItem::ImguiMenuItem()
	:ImguiMenuItem("") {}

ImguiMenuItem::ImguiMenuItem(const std::string& name)
	: ImguiMenuItem(name, "") {}

ImguiMenuItem::ImguiMenuItem(const std::string& name, const std::string& shortcut)
	: ImguiMenuItem(name, shortcut, BUTTON_MITEM) {}

ImguiMenuItem::ImguiMenuItem(const std::string& name, MenuItemType _type)
	: ImguiMenuItem(name, "", _type) {}

ImguiMenuItem::ImguiMenuItem(const std::string& name, const std::string& shortcut, MenuItemType _type)
	: mitem_name(name), mitem_shortcut(shortcut), mitem_type(_type)
{
	if (_type == BOOL_MITEM) {
		mitem_func = [&] {
			//if (tar_state)
			//	*tar_state = !*tar_state;
			return true;
		};
	}
	else {
		mitem_func = [&] {
			DEBUG(mitem_shortcut)
				return true;
		};
	}
}

void ImguiMenuItem::BindSwitch(bool* _switch)
{
	assert(mitem_type == BOOL_MITEM);

	tar_state = std::shared_ptr<bool>(_switch);
}
