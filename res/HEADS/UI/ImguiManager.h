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

/*#define ParaUpdate ParaUpdate*/

class ImguiManager
{
private:
	mutable std::vector<ImguiLayer> layer_list;
	mutable std::vector<ImguiMenu> menu_list;
	ImGuiIO& io=ImGui::GetIO();
	ImGuiStyle& style=ImGui::GetStyle();

	GLFWwindow* window=nullptr;
public:


	ImguiManager();
	ImguiManager(GLFWwindow* window);
	~ImguiManager();

	void ManagerInit(GLFWwindow* window);
	void SetConfigFlag(ImGuiConfigFlags_ flag) const { io.ConfigFlags |= flag; };
	void SetBackendFlag(ImGuiBackendFlags_ flag) const { io.BackendFlags |= flag; };
	ImGuiIO& GetIO()const { return io; }
	ImGuiStyle& GetStyle()const { return style; }

	void DefultViewports();

	void NewFrame() const;
	void GetCurrentWindow() { window = glfwGetCurrentContext(); }

	void PushImguiLayer(const ImguiLayer& layer);
	mutable int active_layer_id;
	mutable std::unordered_map<std::string, int> layer_name_buffer;  //name | ID
	mutable std::unordered_map<std::string, int> menu_name_buffer;   //name | ID

	void SetActiveImguiLayer(const std::string& name)const;
	ImguiLayer* GetActiveImguiLayer()const;
	ImguiLayer* FindImguiLayer(const std::string& name)const;
	ImguiLayer* FindImguiLayer(int id)const;
	ImguiItem* FindImguiItem(const std::string& layer, const std::string& name) const;
	ImguiItem* FindImguiItem(int id, const std::string& name) const;
	ImguiItem* FindImguiItem(int id, int item_id) const;


	void PushImguiMenu(const ImguiMenu& Menu);
	ImguiMenu* FindImguiMenu(const std::string& name)const;

	void SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func);
	Parameters* GetParaValue(const std::string& ly_name, const std::string& it_name);

	mutable std::function<void(void)> ParaUpdate = [] {};

	void RenderUI() const;
};

