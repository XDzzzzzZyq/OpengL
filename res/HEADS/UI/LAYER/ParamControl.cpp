#include "ParamControl.h"

ParamControl::ParamControl()
{

}

ParamControl::ParamControl(const std::string& name)
{
	uly_name = name;
}


ParamControl::~ParamControl()
{

}

void ParamControl::RenderLayer() const
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

		ImGui::End();
	}
	else {
		uly_is_rendered = false;
		ImGui::End();
	}

}
