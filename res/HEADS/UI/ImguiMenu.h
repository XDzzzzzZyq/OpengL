#pragma once
#include <iostream>

#include "ImguiMenuItem.h"

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
	mutable std::vector<ImguiMenuItem*> subm_list;
	void PushSubMenu(ImguiMenuItem* subm);

	void RenderMenu() const;
};

