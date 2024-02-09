#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "EventListener.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"

#define CallBack(func) [&](bool) -> bool {func(); return true;}

enum MenuItemType
{
	NONE_MITEM, BUTTON_MITEM, BOOL_MITEM, OPTION_MITEM
};

class ImguiMenuItem
{
public:
	std::string mitem_name = "";
	std::string mitem_shortcut = "";
	MenuItemType mitem_type = BUTTON_MITEM;

public:
	bool mitem_press{ false };
	bool mitem_onclick_b{ false };
	bool mitem_onclick{ false };
	bool mitem_enable = true;

	std::function<bool(bool)> mitem_func; // function callback

public:
	void EnableMenuItem(bool en) { mitem_enable = en; }

public:
	ImguiMenuItem();
	~ImguiMenuItem() {};
	ImguiMenuItem(const std::string& name);
	ImguiMenuItem(const std::string& name, const std::string& shortcut);
	ImguiMenuItem(const std::string& name, MenuItemType _type);
	ImguiMenuItem(const std::string& name, const std::string& shortcut, MenuItemType _type);

	virtual void BindOption(char* _option) { assert(false && "Not a option item\n"); };
	virtual void BindSwitch(bool* _switch) { assert(false && "Not a switch item\n"); };

	template<typename T> requires std::is_enum_v<T>
	void BindOption(T* _option, std::function<bool(bool)> _callback=NULL);

	virtual void RenderMenuItem() { assert(false && "no Render function overrided\n"); };
};


template<typename T> requires std::is_enum_v<T>
void ImguiMenuItem::BindOption(T* _option, std::function<bool(bool)> _callback/* = NULL*/)
{
	BindOption((char*)_option);
	mitem_func = _callback;
}

