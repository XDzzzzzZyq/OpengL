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
	layer_list.insert(std::pair<std::string, ImguiLayer>(layer.uly_name, layer));
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

void ImguiManager::SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func)
{
	FindImguiLayer("test layer")->FindImguiItem("testB")->ButtonFunc = func;
}

Parameters* ImguiManager::GetParaValue(const std::string& ly_name, const std::string& it_name)
{

}

void ImguiManager::RenderUI() const
{
	for (const auto& menu : menu_list) {
			//menu.second.RenderMenu();
	}


	for (const auto& layer : layer_list) {
		if (layer.second.uly_is_rendered)
			if(layer.first!= ACTIVE)
				layer.second.RenderLayer();
	}

}
