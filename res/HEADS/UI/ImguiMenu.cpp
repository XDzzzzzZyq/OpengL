#include "ImguiMenu.h"

ImguiMenu::ImguiMenu()
{

}

ImguiMenu::ImguiMenu(const std::string& name)
{
	menu_name = name;
}

ImguiMenu::~ImguiMenu()
{
	//for (auto& item : subm_list)
	//	delete item.second;
}

void ImguiMenu::PushSubMenu(ImguiMenuItem* subm)
{
	name_order[subm->mitem_name] = subm_list.size();
	subm_list.push_back(subm);
}

void ImguiMenu::RenderMenu() const
{
	if (ImGui::BeginMenu(menu_name.c_str()))
	{

		for (auto& item : subm_list) {
			switch (item->mitem_type)
			{
			case NONE_MITEM:break;
			case BUTTON_MITEM:
				if (item->mitem_press = ImGui::MenuItem(
					item->mitem_name.c_str(),
					item->mitem_shortcut.c_str(),
					false,
					item->mitem_enable
				)) {
					item->mitem_func();
				};
				break;
			case BOOL_MITEM:
				if (ImGui::MenuItem(
					item->mitem_name.c_str(),
					item->mitem_shortcut.c_str(),
					*item->tar_state,
					item->mitem_enable
				)) {
					item->mitem_func();
				}
				break;
			default:
				break;
			}
		}


		ImGui::EndMenu();
	}
}
