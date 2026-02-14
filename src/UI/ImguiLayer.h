#pragma once

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"

#include "Context.h"
#include "Events.h"

#include "ImguiTheme.h"
#include "ImguiItem.h"

#include <unordered_map>
#define ACTIVE "ACTIVE LAYER"

enum ImLayerType
{
	NONE_UILAYER, 
	PARAS_UILAYER, 
	TOOLS_UILAYER, 
	VIEWPORT_UILAYER, 
	OUTLINER_UILAYER, 
	SHADER_EDIT_UILAYER, 
	MATERIAL_UILYER, 
	TRANSFORM_UILAYER, 
	RENDER_CONFIG_ULATER
};

class ImguiLayer
{
public:
	ImVec2 content_pos;
	ImVec2 content_size;
	
public:
	ImguiLayer();
	ImguiLayer(const std::string& name);
	virtual ~ImguiLayer();
public:
	std::string uly_name;
	GLuint uly_ID = -1;
	ImLayerType uly_type = NONE_UILAYER;
	std::vector<std::shared_ptr<ImguiItem>> item_list;

private:
	mutable std::unordered_map<std::string, int> item_name_buffer;

public:
	bool using_size = false;
	bool fixed_size = false;
	mutable bool is_size_changed = false; // TODO: better design
	mutable bool is_size_changed_b = true;
	bool IsResizingFin() const { return (is_size_changed == false) && (is_size_changed_b == true); }
	ImVec2 uly_size;
	ImVec2 uly_size_b;
	ImVec2 GetLayerSize();
	void UpdateLayerPos();

public:
	bool is_docking = true;
	
	template<class ItemType, class... Args>
	void PushItem(Args... args);
	void PushItem(std::shared_ptr<ImguiItem> item);
	void PushItem(ImItemType type);      //quick push
	ImguiItem* FindImguiItem(const std::string& name) const;
	ImguiItem* FindImguiItem(int id)const;   //start with 0

	bool uly_activate = true;
	bool uly_is_rendered = true;

public:

	bool is_mouse_hovered = false;

public:
	bool uly_show_type = false;
	std::function<void(void)> resize_event = [] {}; // TODO: Event system
	virtual void RegisterEvents(EventPool& evt) {};
	virtual void RenderLayer(const SceneContext& ctx, const EventPool& evt) {};
	virtual void UpdateLayer(const SceneContext& ctx) {};

	// TODO: better design
	void EventInit();
};

template<class ItemType, class... Args>
void ImguiLayer::PushItem(Args... args)
{
	std::shared_ptr<ItemType> item = std::make_shared<ItemType>(args...);
	PushItem(std::dynamic_pointer_cast<ImguiItem>(item));
}

#include <type_traits>

template<typename T>
concept ImguiLayerType = std::is_base_of_v<ImguiLayer, T>;