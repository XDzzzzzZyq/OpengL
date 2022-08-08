#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "support.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "ImguiMenuItem.h"

class ImguiMenu
{
public:
	ImguiMenu();
	ImguiMenu(const std::string& name);
	~ImguiMenu();
	
	mutable std::string menu_name = "";
	mutable int menu_id;

	std::map<std::string, ImguiMenuItem> subm_list;
	void PushSubMenu(const ImguiMenuItem& subm);

	void RenderMenu() const;
	
};

