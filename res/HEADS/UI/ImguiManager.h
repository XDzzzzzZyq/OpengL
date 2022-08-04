#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "ImguiLayer.h"
#include "ImguiMenu.h"

#include <vector>
#include <map>

class ImguiManager
{
private:
	mutable std::map<std::string, ImguiLayer> layer_list;
	mutable std::map<std::string, ImguiMenu> menu_list;
	ImGuiIO& io=ImGui::GetIO();
	ImGuiStyle& style=ImGui::GetStyle();
public:


	ImguiManager();
	~ImguiManager();

	void ManagerInit(GLFWwindow* window);
	void SetConfigFlag(ImGuiConfigFlags_ flag) const { io.ConfigFlags |= flag; };
	void SetBackendFlag(ImGuiBackendFlags_ flag) const { io.BackendFlags |= flag; };
	ImGuiIO& GetIO()const { return io; }
	ImGuiStyle& GetStyle()const { return style; }

	void DefultViewports();

	void NewFrame() const;

	void PushImguiLayer(const ImguiLayer& layer);
	void SetActiveImguiLayer(const std::string& name)const;
	ImguiLayer* GetActiveImguiLayer()const;
	ImguiLayer* FindImguiLayer(const std::string& name)const;


	void SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func);
	Parameters* GetParaValue(const std::string& ly_name, const std::string& it_name);

	void RenderUI() const;
};

