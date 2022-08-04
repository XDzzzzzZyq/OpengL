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
	int repeat_count = 0;
	std::string name = item.uitm_name;
	while (item_list.find(item.uitm_name)!=item_list.end())
	{
		repeat_count++;
		item.Rename(name + ("." + std::to_string(repeat_count)));
		
	}
	item_list.insert(std::pair<std::string, ImguiItem>(item.uitm_name, item));
}

void ImguiLayer::PushItem(ImItemType type)
{
	ImguiItem item(type,"123");
	PushItem(item);
}

ImguiItem* ImguiLayer::FindImguiItem(const std::string& name) const
{
	if (item_list.find(name) != item_list.end())
		return &item_list[name];
	DEBUG("[ no item named "+name+" ]")
	return nullptr;
}

void ImguiLayer::RenderLayer() const
{
	ImGui::Begin(uly_name.c_str());

	for (const auto& item : item_list) {

		if (uly_show_type) {
			item.second.EnableTagName();
		}
		else {
			item.second.DisableTagName();
		}

		switch (item.second.uitm_type)
		{
		default:
			break;
		case NONE_INP:
			break;
		case TEXT_INP:
			ImGui::Text(item.second.GetCharName());
			break;
		case FLOAT_INP:
			ImGui::SliderFloat(

				item.second.GetCharName(),
				&item.second.uitm_para.para_data.fdata,
				item.second.uitm_para.para_data.data_range[0],
				item.second.uitm_para.para_data.data_range[1]
			
			);

			break;
		case RGB_INP:
			ImGui::ColorEdit3(

				item.second.GetCharName(),
				item.second.uitm_para.para_data.v3data
			
			);

			break;
		case BUTTON_INP:
			if (ImGui::Button(item.second.GetCharName()))
				if(item.second.ButtonFunc)
					item.second.ButtonFunc();

			break;
		case IMAGE_INP:

			break;
		case BOOL_INP:
			ImGui::Text(item.second.GetCharName());
			break;
		}
	}
		

	ImGui::End();
}
