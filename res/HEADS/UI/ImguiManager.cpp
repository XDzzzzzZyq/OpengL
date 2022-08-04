#include "ImguiManager.h"

ImguiManager::ImguiManager()
{

	layer_list[ACTIVE] = ImguiLayer(); //Active slot

	io = ImGui::GetIO(); (void)io;
	
	DefultViewports();
}

ImguiManager::~ImguiManager()
{

}

void ImguiManager::ManagerInit(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
}

void ImguiManager::NewFrame() const
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImguiManager::PushImguiLayer(const ImguiLayer& layer)
{
	layer_list[layer.uly_name] = layer;
	layer_list[ACTIVE] = layer;
}

void ImguiManager::SetActiveImguiLayer(const std::string& name) const
{
	
}

ImguiLayer* ImguiManager::GetActiveImguiLayer() const
{
	return &layer_list[ACTIVE];
}

ImguiLayer* ImguiManager::FindImguiLayer(const std::string& name) const
{
	if(layer_list.find(name)!=layer_list.end())
		return &layer_list[name];
	DEBUG("[ no layer named " + name + " ]")
	return nullptr;
}

ImguiItem* ImguiManager::FindImguiItem(const std::string& layer, const std::string& name) const
{
	return FindImguiLayer(layer)->FindImguiItem(name);
}

void ImguiManager::PushImguiMenu(const ImguiMenu& Menu)
{
	menu_list[Menu.menu_name] = Menu;
}

ImguiMenu* ImguiManager::FindImguiMenu(const std::string& name) const
{
	if (menu_list.find(name) != menu_list.end())
		return &menu_list[name];
	DEBUG("[ no menu named " + name + " ]")
		return nullptr;
}

void ImguiManager::SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func)
{
	FindImguiLayer("test layer")->FindImguiItem("testB")->ButtonFunc = func;
}

Parameters* ImguiManager::GetParaValue(const std::string& ly_name, const std::string& it_name)
{
	return &FindImguiLayer(ly_name)->FindImguiItem(it_name)->uitm_para;
}

void ImguiManager::RenderUI() const
{
	ImGui::BeginMainMenuBar();
	/*			ImGui::BeginMenuBar();*/
	for (const auto& menu : menu_list) {
		menu.second.RenderMenu();
	}
	/*			ImGui::EndMenuBar();*/
	ImGui::EndMainMenuBar();



	for (const auto& layer : layer_list) {
		if (layer.second.uly_is_rendered)
			if(layer.first!= ACTIVE)
				layer.second.RenderLayer();
	}

}
