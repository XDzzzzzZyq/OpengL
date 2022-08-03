#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "support.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "ImguiSubMenu.h"

class ImguiMenu
{
public:
	ImguiMenu();
	~ImguiMenu();
	
	std::string menu_name = "";

	std::map<std::string, ImguiSubMenu> subm_list;
	void PushSubMenu(const ImguiSubMenu& subm);

	void RenderMenu();
	
};

