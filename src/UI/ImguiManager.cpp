#include "ImguiManager.h"

#include "Guizmo/ImGuizmo.h"

bool ImguiManager::is_prefW_open = false;

ImguiManager::ImguiManager()
{}

ImguiManager::ImguiManager(GLFWwindow* window)
	:window(window)
{}

void ImguiManager::Init()
{
	active_layer_id = 0;

	io = &ImGui::GetIO(); (void)io;
	m_style = &ImGui::GetStyle();

	DefultViewports();
	DefultEvents();
	RegistarMenuEvents();

	ShaderEditor::InitEditors();
}

void ImguiManager::RegistarMenuEvents()
{
	for(auto& menu : menu_list)
		for(auto& submenu : menu->subm_list){
		
			if (submenu->mitem_shortcut.empty()) continue;
		
			EventList[EventListener::ParseShortCut(submenu->mitem_shortcut)] = [submenu] {
				if(EventListener::is_key_changed)
					submenu->mitem_func(true);
				};
		}
}

void ImguiManager::ManagerInit(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	
	ImGui::GetStyle().Colors[ImGuiCol_Header]			= ImVec4(1, 1, 1, 0);
	ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]		= ImVec4(1, 1, 1, 0.4);
	ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]	= ImVec4(1, 1, 1, 0.2);
	ImGui::GetStyle().Colors[ImGuiCol_Text]				= ImVec4(1, 1, 1, 1);
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg]			= ImVec4(0.06, 0.06, 0.07, 1);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBg]			= ImVec4(0, 0.21, 0.51, 0.51);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]		= ImVec4(0.71, 0.84, 0.96, 1);
	ImGui::GetStyle().Colors[ImGuiCol_DockingEmptyBg]	= ImVec4(0.1, 0.1, 0.1, 1);

	ImGui::GetStyle().WindowRounding	= 0.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().ChildRounding		= 5.0f;
	ImGui::GetStyle().FrameRounding		= 3.0f;
	ImGui::GetStyle().GrabRounding		= 100.0f;
	ImGui::GetStyle().PopupRounding     = 10.0f;
	ImGui::GetStyle().ScrollbarRounding = 5.0f;
	ImGui::GetStyle().TabRounding		= 7;

	ImGui::GetStyle().FrameBorderSize	= 1;
	ImGui::GetStyle().FramePadding		= ImVec2(10, 4);
	ImGui::GetStyle().ItemSpacing		= ImVec2(5, 4);
	ImGui::GetStyle().ScrollbarSize		= 11;
	ImGui::GetStyle().GrabMinSize		= 7;

	ImFontConfig config;
	//config.MergeMode = true;
	//config.GlyphMinAdvanceX = 13.0f;// Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = { ICON_MIN,ICON_MAX,0 };
	ImguiTheme::th_data.font_data.push_back(ImGui::GetIO().Fonts->AddFontFromFileTTF("res/icon/OpenFontIcons.ttf", 13.0f, &config, icon_ranges));

	//EventList[GenIntEvent(0, 0, 0, 3, 0)] = [] {DEBUG(EventListener::EVT_NK_LIST)};
}

void ImguiManager::NewFrame() const
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void ImguiManager::PushImguiLayer(std::shared_ptr<ImguiLayer> layer)
{
	assert(layer_name_buffer.find(layer->uly_name) == layer_name_buffer.end());

	layer->uly_ID = layer_list.size(); //start with 0
	layer_list.push_back(layer);
	active_layer_id = layer->uly_ID;
	layer_name_buffer[layer->uly_name] = layer->uly_ID;

	auto window = std::make_shared<UI::ImguiMSwitch>(layer->uly_name);
	window->BindSwitch(&layer->uly_is_rendered);
	FindImguiMenu("Window")->PushSubMenu(window);
}

void ImguiManager::SetActiveImguiLayer(const std::string& name) const
{
	if (layer_name_buffer.find(name) == layer_name_buffer.end())
		return;
	active_layer_id = layer_name_buffer[name];
}

ImguiLayer* ImguiManager::GetActiveImguiLayer() const
{
	return layer_list[active_layer_id].get();
}

ImguiLayer* ImguiManager::FindImguiLayer(const std::string& name) const
{
	if (layer_name_buffer.find(name) != layer_name_buffer.end())
		return layer_list[layer_name_buffer[name]].get();
	DEBUG("[ no layer named " + name + " ]")
		return nullptr;
}

ImguiLayer* ImguiManager::FindImguiLayer(int id) const
{
	if (id > layer_list.size())
		return nullptr;

	return layer_list[id].get();
}

ImguiItem* ImguiManager::FindImguiItem(const std::string& layer, const std::string& name) const
{
	ImguiLayer* layer_ptr = FindImguiLayer(layer);
	if (layer_ptr == nullptr)
		return nullptr;

	return layer_ptr->FindImguiItem(name);
}

ImguiItem* ImguiManager::FindImguiItem(int id, const std::string& name) const
{
	ImguiLayer* layer_ptr = FindImguiLayer(id);
	if (layer_ptr == nullptr)
		return nullptr;

	return layer_ptr->FindImguiItem(name);
}

ImguiItem* ImguiManager::FindImguiItem(int id, int item_id) const
{
	ImguiLayer* layer_ptr = FindImguiLayer(id);
	if (layer_ptr == nullptr)
		return nullptr;

	return layer_ptr->FindImguiItem(item_id);
}

void ImguiManager::PushImguiMenu(std::shared_ptr<ImguiMenu> _menu)
{
	_menu->menu_id = menu_list.size();
	menu_list.push_back(_menu);
	menu_name_buffer[_menu->menu_name] = _menu->menu_id;
}

ImguiMenu* ImguiManager::FindImguiMenu(const std::string& name) const
{
	if (menu_name_buffer.find(name) != menu_name_buffer.end())
		return menu_list[menu_name_buffer[name]].get();
	DEBUG("[ no menu named " + name + " ]")
		return nullptr;
}

ImguiMenuItem* ImguiManager::FindImguiMenuItem(const std::string& menu, const std::string& submenu) const
{
	ImguiMenu* menu_ptr = FindImguiMenu(menu);
	if (menu_ptr == nullptr)
		return nullptr;

	return menu_ptr->FindMenuItem(submenu);
}

std::shared_ptr<ImguiMenu> ImguiManager::CreateImguiMenu(std::string name)
{
	std::shared_ptr<ImguiMenu> menu = std::make_shared<ImguiMenu>(name);
	PushImguiMenu(menu);

	return menu;
}

void ImguiManager::SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func)
{
	if (FindImguiItem(ly_name, it_name)) {
		DEBUG(ly_name + " " + it_name)
			FindImguiItem(ly_name, it_name)->ButtonFunc = func;
	}
	else {
		DEBUG("button not found")
	}

}

Parameters* ImguiManager::GetParaValue(const std::string& ly_name, const std::string& it_name)
{
	ImguiItem* item = FindImguiItem(ly_name, it_name);
	if (item == nullptr)
		return nullptr;

	return item->GetPara();
}

void ImguiManager::RenderUI(bool rend)
{
	if (rend) {

		if (ParaUpdate)
			ParaUpdate();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		ImGui::BeginMainMenuBar();
		EventListener::window_pos = VecConvert<ImVec2, glm::vec2>(ImGui::GetWindowPos());
		/*			ImGui::BeginMenuBar();*/
		for (const auto& menu : menu_list) {
			menu->RenderMenu();
		}
		/*			ImGui::EndMenuBar();*/
		ImGui::EndMainMenuBar();

		//ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 100.0f);
		for (const auto& layer : layer_list) {
			if (layer->uly_ID != active_layer_id) {
				if (!layer->uly_is_rendered)
					continue;
				layer->UpdateLayer();
				layer->RenderLayer();
			}
		}
		if (layer_list[active_layer_id]->uly_is_rendered) {
			layer_list[active_layer_id]->UpdateLayer();
			layer_list[active_layer_id]->RenderLayer();
		}
		//ImGui::PopStyleVar();
		//ImGui::ShowStyleEditor();
	}
	else {
		io->ConfigFlags = !ImGuiConfigFlags_DockingEnable;
	}

	if (is_prefW_open)
		ImGui::ShowStyleEditor();

	ImGui::Render();

	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
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
