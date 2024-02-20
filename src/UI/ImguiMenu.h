#pragma once
#include <iostream>

#include "menu/ImguiMButton.h"
#include "menu/ImguiMOption.h"
#include "menu/ImguiMSwitch.h"

class ImguiMenu
{
private:
	mutable std::unordered_map<std::string, int> name_order;
public:
	ImguiMenu();
	ImguiMenu(const std::string& name);
	~ImguiMenu();

public:
	mutable std::string menu_name = "";
	mutable int menu_id;

public:
	mutable std::vector<std::shared_ptr<ImguiMenuItem>> subm_list;

	template<class MItemType, class... Args>
	void PushSubMenu(Args... args);
	void PushSubMenu(std::shared_ptr<ImguiMenuItem> subm);

	ImguiMenuItem* FindMenuItem(const std::string _name);

	void RenderMenu() const;
};



template<class MItemType, class... Args>
void ImguiMenu::PushSubMenu(Args... args)
{
	std::shared_ptr<MItemType> item = std::make_shared<MItemType>(args...);
	PushSubMenu(std::dynamic_pointer_cast<ImguiMenuItem>(item));
}

