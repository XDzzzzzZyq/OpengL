#pragma once

#include "ImGui/imgui.h"
#include "ImguiLayer.h"
#include <vector>

class ImguiManager
{
private:
	std::vector<ImguiLayer> layer_list;
public:
	ImguiManager();
	~ImguiManager();


	void PushImguiLayer(ImguiLayer layer) const;
	void RenderUI() const;
};

