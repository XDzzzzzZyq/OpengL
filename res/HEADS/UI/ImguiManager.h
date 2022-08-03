#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ImGui/imgui.h"
#include "ImguiLayer.h"
#include <vector>
#include <map>

class ImguiManager
{
private:
	std::map<std::string, ImguiLayer> layer_list;
public:
	ImguiManager();
	~ImguiManager();

	void ImguiMngerInit(GLFWwindow* window);


	void PushImguiLayer(const ImguiLayer& layer);
	void RenderUI() const;
};

