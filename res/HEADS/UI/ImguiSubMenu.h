#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "support.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

class ImguiSubMenu {
public:
	std::string subm_name = "";

	ImguiSubMenu();
	~ImguiSubMenu();
};

