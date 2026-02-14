#include "ParamControl.h"

ParamControl::ParamControl()
{

}

ParamControl::ParamControl(const std::string& name)
{
	uly_name = name;
	uly_type = PARAS_UILAYER;
}


ParamControl::~ParamControl()
{

}

void ParamControl::RenderLayer(const SceneContext& ctx, const EventPool& evt)
{
	for (const auto& item : item_list) {
		uly_show_type ? item->EnableTagName() : item->DisableTagName();
		item->RenderItem();
	}
}
