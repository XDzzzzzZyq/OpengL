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
}

ImguiMenuItem* ImguiMenu::FindMenuItem(const std::string _name)
{
	if (name_order.find(_name) == name_order.end())
		return nullptr;

	return subm_list[name_order[_name]].get();
}

void ImguiMenu::RenderMenu() const
{
	if (ImGui::BeginMenu(menu_name.c_str()))
	{

		for (auto& item : subm_list) {
			item->RenderMenuItem();
		}

		ImGui::EndMenu();
	}
	for (auto& item : subm_list) {
		item->mitem_onclick_b = item->mitem_onclick;
		item->mitem_onclick = false;
	}

}
