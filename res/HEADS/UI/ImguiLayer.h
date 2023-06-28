#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"


#include "EventListener.h"
#include "ImguiTheme.h"

#include "ITEM/ParaInput.h"
#include "ITEM/TextureViewer.h"
#include "ITEM/Text.h"
#include "ITEM/Button.h"
#include "ITEM/OpaButton.h"

#include <unordered_map>
#include <map>
#define ACTIVE "ACTIVE LAYER"

enum ImLayerType
{
	NONE_UILAYER, PARAS_UILAYER, TOOLS_UILAYER, VIEWPORT_UILAYER, OUTLINER_UILAYER
};

class ImguiLayer : public EventListener
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
	mutable bool is_size_changed = false;
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
	bool uly_show_type = false;
	std::function<void(void)> pre_RenderLayer = [] {};
	std::function<void(void)> extra_RenderLayer = [] {};
	std::function<void(void)> resize_event = [] {};
	virtual void RenderLayer() { DEBUG("no Render function overrided")return; };
	virtual void UpdateLayer() {};

	//for outline          |  TYPE  |  NAME  |
	virtual void SetObjectList(OutlineData* data){ DEBUG(uly_name + " is not a Outline") return; }
	std::function<void(void)> set_active = [] {};

	void EventInit();
	void LMB();
};

template<class ItemType, class... Args>
void ImguiLayer::PushItem(Args... args)
{
	std::shared_ptr<ItemType> item = std::make_shared<ItemType>(args...);
	PushItem(std::dynamic_pointer_cast<ImguiItem>(item));
}

