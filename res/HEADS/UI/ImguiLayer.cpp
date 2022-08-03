#include "ImguiLayer.h"

ImguiLayer::ImguiLayer()
{
	uly_name = "";
}

ImguiLayer::~ImguiLayer()
{

}

void ImguiLayer::PushItem(const ImguiItem& item)
{
	item_list.insert(std::pair<std::string, ImguiItem>(item.uitm_name, item));
}

void ImguiLayer::PushItem(ImItemType type)
{
	ImguiItem item(type,"123");
	PushItem(item);
}

void ImguiLayer::RenderLayer() const
{
	ImGui::Begin(uly_name.c_str());

	for (const auto& item : item_list) {
		switch (item.second.uitm_type)
		{
		default:
			break;
		case NONE_INP:
			break;
		case TEXT_INP:
			ImGui::Text(item.second.uitm_name.c_str());
			break;
		case FLOAT_INP:
			ImGui::SliderFloat(item.second.uitm_name.c_str(), &item.second.uitm_para.para_data.fdata,-10,10);
			break;
		case RGB_INP:
			ImGui::ColorEdit3(item.second.uitm_name.c_str(), item.second.uitm_para.para_data.v3data);
			break;
		case BUTTON_INP:
			ImGui::Text(item.second.uitm_name.c_str());
			break;
		case IMAGE_INP:
			ImGui::Text(item.second.uitm_name.c_str());
			break;
		case BOOL_INP:
			ImGui::Text(item.second.uitm_name.c_str());
			break;
		}
	}
		

	ImGui::End();
}
