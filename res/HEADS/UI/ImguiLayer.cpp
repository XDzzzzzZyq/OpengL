#include "ImguiLayer.h"

ImguiLayer::ImguiLayer()
{
	uly_name = "";
	EventInit();
}

ImguiLayer::ImguiLayer(const std::string& name)
{
	uly_name = name;
	EventInit();
}

ImguiLayer::~ImguiLayer()
{

}

ImVec2 ImguiLayer::GetLayerSize() const
{
	if (ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() == uly_size) {
		is_size_changed = false;
		return uly_size;
	}
	else
	{
		uly_size = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
		is_size_changed = true;
		return uly_size;
	}


}

void ImguiLayer::PushItem(ImguiItem* item)
{
	int repeat_count = 0;
	std::string name = item->uitm_name;
	while (item_name_buffer.find(item->uitm_name) != item_name_buffer.end())
	{
		repeat_count++;
		item->Rename(name + ("." + std::to_string(repeat_count)));
		
	}
	item->uitm_id = item_list.size();
	item_list.push_back(item);
	item_name_buffer[item->uitm_name] = item->uitm_id;
}

void ImguiLayer::PushItem(ImItemType type)
{
	auto item = new ImguiItem(type,"123");
	PushItem(item);
}

ImguiItem* ImguiLayer::FindImguiItem(const std::string& name) const
{
	if (item_name_buffer.find(name) != item_name_buffer.end())
		return item_list[item_name_buffer[name]];
	DEBUG("[ no item named "+name+" ]")
	return nullptr;
}

ImguiItem* ImguiLayer::FindImguiItem(int id) const
{
	if (id > item_list.size() - 1)
		return nullptr;
	return item_list[id];
}

void ImguiLayer::RenderLayer() const
{
	if (!uly_is_rendered)
		return;

	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {
		if (pre_RenderLayer)
			pre_RenderLayer();

		for (const auto& item : item_list) {
			uly_show_type ? item->EnableTagName() : item->DisableTagName();
			item->RenderItem();
		}
		if (extra_RenderLayer)
			extra_RenderLayer();

		GetLayerSize();
		if (is_size_changed)
			if (resize_event)
				resize_event();

		is_size_changed = false;
		ImGui::End();
	}
	else {
		uly_is_rendered = false;
		ImGui::End();
	}

}

void ImguiLayer::EventInit()
{
	EventList[GenIntEvent(0, 0, 0, 1, 0)] = std::bind(&ImguiLayer::LMB, this);
}

void ImguiLayer::LMB()
{
	if(!is_mouse_pressed)
	DEBUG("CLICK")
		is_mouse_pressed = true;
}
