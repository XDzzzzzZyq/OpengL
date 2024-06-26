#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"

#include "layer/Viewport.h"
#include "layer/Outliner.h"
#include "layer/ParamControl.h"
#include "layer/ShaderEditor.h"
#include "layer/MaterialViewer.h"
#include "layer/TransformPanel.h"
#include "layer/RenderConfigViewer.h"

#include "ImguiMenu.h"
#include "EventListener.h"
#include "ImguiTheme.h"

#include <vector>
#include <map>

/*#define ParaUpdate ParaUpdate*/

class ImguiManager : public EventListener
{
private:
	std::vector<std::shared_ptr<ImguiLayer>> layer_list;
	std::vector<std::shared_ptr<ImguiMenu>> menu_list;
	
	ImGuiIO* io = nullptr;
	ImGuiStyle* m_style= nullptr;
	GLFWwindow* window=nullptr;

	mutable int active_layer_id;
	mutable std::unordered_map<std::string, int> layer_name_buffer;  //name | ID
	mutable std::unordered_map<std::string, int> menu_name_buffer;   //name | ID

	void RegistarMenuEvents();

public:
	static bool is_prefW_open;

public:

	ImguiManager();
	ImguiManager(GLFWwindow* window);
	void Init();
	void _debug() const;

public:
	void ManagerInit(GLFWwindow* window);
	void SetConfigFlag(ImGuiConfigFlags_ flag) const { io->ConfigFlags |= flag; };
	void SetBackendFlag(ImGuiBackendFlags_ flag) const { io->BackendFlags |= flag; };
	ImGuiIO* GetIO()const { return io; }
	ImGuiStyle* GetStyle()const { return m_style; }

	void DefultViewports();
	void DefultEvents();

public:
	void NewFrame() const;
	void RenderUI(bool rend = true);

public:
	void PushImguiMenu(std::shared_ptr<ImguiMenu> _menu);
	std::shared_ptr<ImguiMenu> CreateImguiMenu(std::string name);

	ImguiMenu* FindImguiMenu(const std::string& name) const;
	ImguiMenuItem* FindImguiMenuItem(const std::string& menu, const std::string& submenu) const;

public:
	void PushImguiLayer(std::shared_ptr<ImguiLayer> layer);
	template<class LayerType>
	std::shared_ptr<LayerType> CreateImguiLayer(std::string name);

	void SetActiveImguiLayer(const std::string& name)const;
	ImguiLayer* GetActiveImguiLayer()const;
	ImguiLayer* FindImguiLayer(const std::string& name)const;
	ImguiLayer* FindImguiLayer(int id)const;
	ImguiItem* FindImguiItem(const std::string& layer, const std::string& name) const;
	ImguiItem* FindImguiItem(int id, const std::string& name) const;
	ImguiItem* FindImguiItem(int id, int item_id) const;

public:
	void SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func);
	Parameters* GetParaValue(const std::string& ly_name, const std::string& it_name);
	void GetCurrentWindow() { window = glfwGetCurrentContext(); }

public:
	mutable std::function<void(void)> ParaUpdate = [] {};

};

template<class LayerType>
std::shared_ptr<LayerType> ImguiManager::CreateImguiLayer(std::string name)
{
	std::shared_ptr<LayerType> layer = std::make_shared<LayerType>(name);
	PushImguiLayer(std::dynamic_pointer_cast<ImguiLayer>(layer));

	return layer;
}

