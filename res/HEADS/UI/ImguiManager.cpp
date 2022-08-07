#include "ImguiManager.h"

ImguiManager::ImguiManager()
{
	layer_list[ACTIVE] = ImguiLayer(); //Active slot

	io = ImGui::GetIO(); (void)io;
	
	DefultViewports();
}

ImguiManager::ImguiManager(GLFWwindow* window)
	:window(window)
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
	layer_name_list.push_back(layer.uly_name);
	layer.uly_ID = layer_name_list.size();
}

void ImguiManager::SetActiveImguiLayer(const std::string& name) const
{
	if (layer_list.find(name) == layer_list.end())
		return;
	active_layer_name = name;
	layer_list[ACTIVE] = layer_list[name];
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

ImguiLayer* ImguiManager::FindImguiLayer(int id) const
{
	if (id > layer_name_list.size())
		return nullptr;

	return &layer_list[layer_name_list[id - 1]];
}

ImguiItem* ImguiManager::FindImguiItem(const std::string& layer, const std::string& name) const
{
	return FindImguiLayer(layer)->FindImguiItem(name);
}

ImguiItem* ImguiManager::FindImguiItem(int id, const std::string& name) const
{
	return FindImguiLayer(id)->FindImguiItem(name);
}

ImguiItem* ImguiManager::FindImguiItem(int id, int item_id) const
{
	return FindImguiLayer(id)->FindImguiItem(item_id);
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
	if (FindImguiItem(ly_name, it_name)) {
		DEBUG(ly_name+" "+it_name)
			FindImguiItem(ly_name, it_name)->ButtonFunc = func;
	}
	else {
		DEBUG("button not found")
	}
		
}

Parameters* ImguiManager::GetParaValue(const std::string& ly_name, const std::string& it_name)
{
	return FindImguiLayer(ly_name)->FindImguiItem(it_name)->GetPara();
}

void ImguiManager::RenderUI() const
{
	if (ParaUpdate)
		ParaUpdate();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	ImGui::BeginMainMenuBar();
	/*			ImGui::BeginMenuBar();*/
	for (const auto& menu : menu_list) {
		menu.second.RenderMenu();
	}
	/*			ImGui::EndMenuBar();*/
	ImGui::EndMainMenuBar();



	for (const auto& layer : layer_list) {
		if (layer.second.uly_is_rendered)
			if(layer.first!= ACTIVE && layer.first != active_layer_name)
				layer.second.RenderLayer();
	}
	layer_list[active_layer_name].RenderLayer();

	ImGui::Render();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		if (window) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(window);
		}
		else
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

	}

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
