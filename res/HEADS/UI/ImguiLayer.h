#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"


#include "EventListener.h"

#include "ITEM/ParaInput.h"
#include "ITEM/TextureView.h"
#include "ITEM/Text.h"
#include "ITEM/Button.h"
#include "ITEM/OpaButton.h"

#include <unordered_map>
#include <map>
#define ACTIVE "ACTIVE LAYER"

enum ImLayerType
{
	NONE_UILAYER, PARAS_UILAYER, TOOLS_UILAYER, VIEWPORT_UILAYER
};

class ImguiLayer : public EventListener
{
private:
	mutable ImVec2 content_pos;
	mutable ImVec2 content_size;
	
public:
	ImguiLayer();
	ImguiLayer(const std::string& name);
	virtual ~ImguiLayer();
public:
	std::string uly_name;
	mutable unsigned int uly_ID = -1;
	ImLayerType uly_type = NONE_UILAYER;
	mutable std::vector<ImguiItem*> item_list;
	mutable std::unordered_map<std::string, int> item_name_buffer;

public:
	bool using_size = false;
	bool fixed_size = false;
	mutable bool is_size_changed = false;
	mutable bool is_size_changed_b = true;
	bool IsChangeEnd() const { return (is_size_changed == false) && (is_size_changed_b == true); }
	mutable ImVec2 uly_size;
	mutable ImVec2 uly_size_b;
	ImVec2 GetLayerSize() const;
	void UpdateLayerPos();

public:
	bool is_docking = true;
	
	void PushItem(ImguiItem* item);
	void PushItem(ImItemType type);      //quick push
	ImguiItem* FindImguiItem(const std::string& name)const;
	ImguiItem* FindImguiItem(int id)const;   //start with 0

	mutable bool uly_activate = true;
	mutable bool uly_is_rendered = true;

public:
	bool uly_show_type = false;
	mutable std::function<void(void)> pre_RenderLayer = [] {};
	mutable std::function<void(void)> extra_RenderLayer = [] {};
	mutable std::function<void(void)> resize_event = [] {};
	virtual void RenderLayer() const { DEBUG("no Render function overrided")return; };
	virtual void UpdateLayer() {};

	//for outline          |  TYPE  |  NAME  |
	virtual void SetObjectList(OutlineData* data){ DEBUG(uly_name + " is not a Outline") return; }
	mutable std::function<void(void)> set_active = [] {};

	void EventInit();
	void LMB();
};

