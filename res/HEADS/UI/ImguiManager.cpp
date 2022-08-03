#include "ImguiManager.h"

ImguiManager::ImguiManager()
{

	//layer_list["__ACTIVE SLOT"] = ImguiLayer(); //Active slot
	io = ImGui::GetIO(); (void)io;
	ImguiLayer layer1;
	layer1.uly_name = "test layer";
	layer1.PushItem(ImguiItem(FLOAT_INP, "test1"));
	layer1.PushItem(ImguiItem(FLOAT_INP, "test2"));
	layer1.PushItem(ImguiItem(FLOAT_INP, "test3"));
	layer1.PushItem(ImguiItem(RGB_INP, "test4"));
	
	PushImguiLayer(layer1);
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
}
void ImguiManager::RenderUI() const
{
	for (const auto& menu : menu_list) {
			//menu.second.RenderMenu();
	}


	for (const auto& layer : layer_list) {
		if (layer.second.uly_is_rendered)
			layer.second.RenderLayer();

	}


}
