#pragma once

#include "ImGui/imgui.h"
#include "ImguiItem.h"
#include <unordered_map>
#include <map>

enum ImLayerType
{
	NONE_UILAYER, PARAS_UILAYER, TOOLS_UILAYER
};

class ImguiLayer
{
private:
	std::map<std::string, ImguiItem> item_list;
public:
	ImguiLayer();
	ImguiLayer(const std::string& name);
	~ImguiLayer();

	std::string uly_name;
	unsigned int uly_ID = 0;

	ImLayerType uly_type = NONE_UILAYER;
	
	
	void PushItem(const ImguiItem& item);
	void PushItem(ImItemType type);      //quick push

	bool uly_activate = true;
	bool uly_is_rendered = true;
	void RenderLayer() const;
};

