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

void ImguiMenu::PushSubMenu(std::shared_ptr<ImguiMenuItem> subm)
{
	name_order[subm->mitem_name] = subm_list.size();
	subm_list.push_back(subm);
	if (subm->mitem_shortcut.size())
		mitm_func_list[EventListener::ParseShortCut(subm->mitem_shortcut)] = [subm] {
		subm->mitem_onclick = true;
	};

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
	for (auto& item : subm_list) {

		if ((item->mitem_onclick_b == false) && (item->mitem_onclick == true))
			item->mitem_func();

		item->mitem_onclick_b = item->mitem_onclick;
		item->mitem_onclick = false;
	}

}
