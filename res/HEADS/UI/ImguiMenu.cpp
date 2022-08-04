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

}

void ImguiMenu::PushSubMenu(const ImguiMenuItem& subm)
{
	subm_list[subm.subm_name] = subm;
}

void ImguiMenu::RenderMenu() const
{
	if (ImGui::BeginMenu(menu_name.c_str()))
	{

		for (const auto& item : subm_list)
			ImGui::MenuItem(
				item.second.subm_name.c_str(),
				item.second.subm_shortcut.c_str(),
				&item.second.subm_press,
				item.second.subm_enable
				);

		ImGui::EndMenu();
	}
}
