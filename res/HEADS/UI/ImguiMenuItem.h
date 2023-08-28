#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "EventListener.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

enum MenuItemType
{
	NONE_MITEM, BUTTON_MITEM, BOOL_MITEM
};

class ImguiMenuItem 
{
public:
	std::string mitem_name = "";
	std::string mitem_shortcut = "";
	MenuItemType mitem_type = BUTTON_MITEM;

public:
	mutable bool mitem_press{false};
	mutable bool mitem_onclick_b{false};
	mutable bool mitem_onclick{ false };
	bool mitem_enable = true;

	std::shared_ptr<bool> tar_state;
	mutable std::function<bool(void)> mitem_func; //return onopen state;

public:
	void EnableMenuItem(bool en) { mitem_enable = en; }
	void ListenEvent();

public:
	ImguiMenuItem();
	ImguiMenuItem(const std::string& name);
	ImguiMenuItem(const std::string& name, const std::string& shortcut);
	ImguiMenuItem(const std::string& name, MenuItemType _type);
	ImguiMenuItem(const std::string& name, const std::string& shortcut, MenuItemType _type);
};

