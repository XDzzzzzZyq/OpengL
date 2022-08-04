#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "support.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

class ImguiMenuItem {
public:
	std::string subm_name = "";
	std::string subm_shortcut = "";

	bool subm_press;
	bool subm_enable = false;
	mutable std::function<void(void)> subm_func;

	void EnableMenuItem(bool en) { subm_enable = en; }
	void ListenEvent();

	ImguiMenuItem();
	ImguiMenuItem(const std::string& name);
	ImguiMenuItem(const std::string& name, const std::string& shortcut);
	~ImguiMenuItem();
};

