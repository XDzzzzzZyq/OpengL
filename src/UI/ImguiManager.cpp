#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"

#include "ImguiManager.h"
#include "ImguiTheme.h"
#include "Input.h"
#include "layer/ShaderEditor.h"
#include "Guizmo/ImGuizmo.h"

#include "events/KeyMouseEvents.h"

#include "../app/Window.h"

bool ImguiManager::is_prefW_open = false;

ImguiManager::ImguiManager(EventPool& evt, Window& w)
{
	// Create the ImGui context; must happen before any ImGui::GetIO() / GetStyle() calls.
	// Moved here from Application so ImguiManager fully owns its context lifetime.
	ImGui::CreateContext();

	io = &ImGui::GetIO();
	m_style = &ImGui::GetStyle();

	DefultViewports();
	RegistarMenuEvents(evt);
	RegisterLayerEvents(evt);

	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(w.Get(), true);
	ImGui::StyleColorsDark();

	ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(1, 1, 1, 0);
	ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(1, 1, 1, 0.4f);
	ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(1, 1, 1, 0.2f);
	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.07f, 1);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0, 0.21f, 0.51f, 0.51f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.71f, 0.84f, 0.96f, 1);
	ImGui::GetStyle().Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.1f, 0.1f, 0.1f, 1);

	ImGui::GetStyle().WindowRounding = 0.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().ChildRounding = 5.0f;
	ImGui::GetStyle().FrameRounding = 3.0f;
	ImGui::GetStyle().GrabRounding = 100.0f;
	ImGui::GetStyle().PopupRounding = 10.0f;
	ImGui::GetStyle().ScrollbarRounding = 5.0f;
	ImGui::GetStyle().TabRounding = 7;

	ImGui::GetStyle().FrameBorderSize = 1;
	ImGui::GetStyle().FramePadding = ImVec2(10, 4);
	ImGui::GetStyle().ItemSpacing = ImVec2(5, 4);
	ImGui::GetStyle().ScrollbarSize = 11;
	ImGui::GetStyle().GrabMinSize = 7;

	ImFontConfig config;
	//config.MergeMode = true;
	//config.GlyphMinAdvanceX = 13.0f;// Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = { ICON_MIN,ICON_MAX,0 };
	ImguiTheme::th_data.font_data.push_back(ImGui::GetIO().Fonts->AddFontFromFileTTF("res/icon/OpenFontIcons.ttf", 13.0f, &config, icon_ranges));
}

ImguiManager::~ImguiManager()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImguiManager::_debug() const
{
	for (auto& [name, id] : layer_name_buffer) {
		std::cout << name << " " << id << "\n";
	}
}

void ImguiManager::RegistarMenuEvents(EventPool& evt)
{
	for (auto& menu : menu_list)
		for (auto& submenu : menu->subm_list) {
			if (submenu->mitem_shortcut.empty()) continue;

			Input::KeyState key_state = Input::ParseKeyState(submenu->mitem_shortcut);
			evt.subscribe<KeyClickEvent>([submenu, key_state](KeyClickEvent e) {
				if (e.key == key_state)	submenu->mitem_func(true);
				});
		}
}



void ImguiManager::RegisterLayerEvents(EventPool& evt)
{
	for (auto& layer : layer_list) {
		layer->RegisterEvents(evt);
	}
}

void ImguiManager::NewFrame() const
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

#include "menu/ImguiMSwitch.h"
void ImguiManager::PushImguiLayer(std::shared_ptr<ImguiLayer> layer)
{
	assert(layer_name_buffer.find(layer->uly_name) == layer_name_buffer.end());

	layer->uly_ID = (int)layer_list.size(); //start with 0
	layer_list.push_back(layer);
	layer_name_buffer[layer->uly_name] = layer->uly_ID;

	auto window = std::make_shared<UI::ImguiMSwitch>(layer->uly_name);
	window->BindSwitch(&layer->uly_is_rendered);
	FindImguiMenu("Window")->PushSubMenu(window);
}

ImguiLayer* ImguiManager::FindImguiLayer(const std::string& name) const
{
	if (layer_name_buffer.find(name) != layer_name_buffer.end())
		return layer_list[layer_name_buffer[name]].get();
	DEBUG("[ no layer named " + name + " ]");
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
	_menu->menu_id = (int)menu_list.size();
	menu_list.push_back(_menu);
	menu_name_buffer[_menu->menu_name] = _menu->menu_id;
}

ImguiMenu* ImguiManager::FindImguiMenu(const std::string& name) const
{
	if (menu_name_buffer.find(name) != menu_name_buffer.end())
		return menu_list[menu_name_buffer[name]].get();
	DEBUG("[ no menu named " + name + " ]");
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

void ImguiManager::ActivateLayer(const std::string& name)
{
	auto it = layer_name_buffer.find(name);
	if (it == layer_name_buffer.end())
		return;

	int idx = it->second;

	if (idx == layer_list.size() - 1)
		return; // already active

	auto layer = layer_list[idx];
	layer_list.erase(layer_list.begin() + idx);

	// update name mapping
	for (auto& [n, i] : layer_name_buffer)
		if (i > idx)
			--i;

	layer_list.push_back(layer);
	layer_name_buffer[name] = layer_list.size() - 1;
}

void ImguiManager::SetButtonFunc(const std::string& ly_name, const std::string& it_name, const std::function<void(void)>& func)
{
	if (FindImguiItem(ly_name, it_name)) {
		DEBUG(ly_name + " " + it_name);
		FindImguiItem(ly_name, it_name)->ButtonFunc = func;
	}
	else {
		DEBUG("button not found");
	}

}

Parameters* ImguiManager::GetParaValue(const std::string& ly_name, const std::string& it_name)
{
	ImguiItem* item = FindImguiItem(ly_name, it_name);
	if (item == nullptr)
		return nullptr;

	return item->GetPara();
}

void RenderLayer(ImguiLayer* layer, const Context& ctx, EventPool& evt)
{
	if (layer->uly_is_rendered) {
		if (ImGui::Begin(layer->uly_name.c_str(), &layer->uly_is_rendered)) {
			const ImVec2 layer_size = layer->GetLayerSize();
			const ImVec2 layer_pos = layer->GetLayerPos();
			const ImVec2 mouse_pos = ImVec2(Input::GetMousePosX(), Input::GetMousePosY());
			layer->is_mouse_hovered = Item::is_inside(layer_size, mouse_pos);

			layer->RenderLayer(ctx, evt);

			layer->is_size_changed_b = layer->is_size_changed;
		}
		else {
			layer->is_mouse_hovered = false;
		}
		ImGui::End();
	}
	else {
		layer->is_mouse_hovered = false;
	}
}

void ImguiManager::RenderUI(const Context& ctx, EventPool& evt, bool rend)
{
	if (rend) {
		
		if (ParaUpdate)
			ParaUpdate();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
		ImGui::BeginMainMenuBar();
		/*			ImGui::BeginMenuBar();*/
		for (const auto& menu : menu_list) {
			menu->RenderMenu(ctx);
		}
		/*			ImGui::EndMenuBar();*/
		ImGui::EndMainMenuBar();
		
		for (const auto& layer : layer_list) {
			::RenderLayer(layer.get(), ctx, evt);
		}
	}
	else {
		io->ConfigFlags = !ImGuiConfigFlags_DockingEnable;
	}

	if (is_prefW_open)
		ImGui::ShowStyleEditor();
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwSetErrorCallback([](int code, const char* desc) {
			fprintf(stderr, "GLFW Error %d: %s\n", code, desc);
			});
		//glfwMakeContextCurrent(window);
	}
}
