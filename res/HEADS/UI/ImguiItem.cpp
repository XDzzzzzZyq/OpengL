#include "ImguiItem.h"

ImguiItem::ImguiItem()
{

}

ImguiItem::ImguiItem(ImItemType type, const std::string& name)
{
	uitm_type = type;
	notagname = uitm_name = name;
	switch (type)
	{
	case NONE_INP:
		uitm_name = "NONE";
		break;
	case TEXT_INP:

		uitm_para = Parameters(INT_PARA);
		break;
	case FLOAT_INP:

		uitm_para = Parameters(FLOAT_PARA);
		break;
	case RGB_INP:

		uitm_para = Parameters(VEC3_PARA);
		break;
	case BUTTON_INP:

		break;
	case IMAGE_INP:

		break;
	case BOOL_INP:

		uitm_para = Parameters(BOOL_PARA);
		break;
	default:
		break;
	}
}

ImguiItem::ImguiItem(const Parameters& para)
{
	uitm_para = para;
	uitm_name = para.para_name;

	switch (para.para_type)
	{
	default:
		break;
	case NONE_PARA:
		uitm_type = NONE_INP;
		break;
	case FLOAT_PARA:
		uitm_type = FLOAT_INP;
		break;
 	case INT_PARA:
 		uitm_type = FLOAT_INP;
		break;
	case STRING_PARA:
		uitm_type = TEXT_INP;
		break;
// 	case VEC2_PARA:
// 		break;
	case VEC3_PARA:
		uitm_type = RGB_INP;
		break;
 	case VEC4_PARA:
		uitm_type = RGBA_INP;
// 		break;
	}
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
	case IMAGE_INP:

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
