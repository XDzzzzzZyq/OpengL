#pragma once

#include "ImGui/imgui.h"
#include "ImguiItem.h"
#include <unordered_map>
#include <vector>

enum ImLayerType
{
	NONE_UILAYER, PARAS_UILAYER, TOOLS_UILAYER
};

class ImguiLayer
{
private:
	std::vector<ImguiItem> item_list;
public:
	ImguiLayer();
	~ImguiLayer();

	std::string uly_name = "";
	unsigned int uly_ID = 0;

	ImLayerType uly_type = NONE_UILAYER;
	
	
	void PushItem(const ImguiItem& item);
	void PushItem(ImItemType type);      //quick push

	void RenderLayer() const;
};

