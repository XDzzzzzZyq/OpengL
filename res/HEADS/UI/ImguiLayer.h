#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImguiItem.h"
#include <unordered_map>
#include <map>

enum ImLayerType
{
	NONE_UILAYER, PARAS_UILAYER, TOOLS_UILAYER, VIEWPORT_UILAYER
};

class ImguiLayer
{
	
	
public:
	ImguiLayer();
	ImguiLayer(const std::string& name);
	~ImguiLayer();

	std::string uly_name;
	unsigned int uly_ID = 0;

	bool using_size = false;
	bool fixed_size = false;
	ImVec2 uly_size;

	bool is_docking = true;

	ImLayerType uly_type = NONE_UILAYER;
	mutable std::map<std::string, ImguiItem> item_list;
	
	void PushItem(const ImguiItem& item);
	void PushItem(ImItemType type);      //quick push
	ImguiItem* FindImguiItem(const std::string& name)const;

	bool uly_activate = true;
	bool uly_is_rendered = true;
	bool uly_show_type = false;
	void RenderLayer() const;
};

