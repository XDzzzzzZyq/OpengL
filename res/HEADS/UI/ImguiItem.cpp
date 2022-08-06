#include "ImguiItem.h"

ImguiItem::ImguiItem()
{

}

ImguiItem::ImguiItem(ImItemType type, const std::string& name)
{
	uitm_type = type;
	notagname = uitm_name = name;
}

ImguiItem::~ImguiItem()
{

}

void ImguiItem::Rename(const std::string& name) const
{
	uitm_name = name;
	notagname = name;
}

std::string ImguiItem::GetTagName() const
{
	switch (uitm_type)
	{
	case NONE_INP:

		break;
	case TEXT_INP:

		return "[TEXT] ";
		break;
	case FLOAT_INP:

		return "[FLOAT] ";
		break;
	case RGB_INP:

		return "[RGB] ";
		break;
	case BUTTON_INP:

		return "[BUTTON] ";
		break;
	case IMAGE_OUTP:

		return "[IMAGE] ";
		break;
	case BOOL_INP:

		return "[BOOL] ";
		break;
	default:
		break;
	}
}

void ImguiItem::EnableTagName() const
{
	if (!show_tag_name) {
		uitm_name = GetTagName() + uitm_name;
		show_tag_name = true;
	}

}

void ImguiItem::DisableTagName() const
{

	uitm_name = notagname;
	show_tag_name = false;

}
