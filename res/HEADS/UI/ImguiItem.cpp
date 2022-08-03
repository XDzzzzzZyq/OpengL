#include "ImguiItem.h"

ImguiItem::ImguiItem()
{

}

ImguiItem::ImguiItem(ImItemType type, const std::string& name)
{
	uitm_type = type;

	switch (type)
	{
	case NONE_INP:
		uitm_name = "NONE";
		break;
	case TEXT_INP:
		uitm_name = "TEXT : "+name;
		uitm_para = Parameters(INT_PARA);
		break;
	case FLOAT_INP:
		uitm_name = "FLOAT : " + name;
		uitm_para = Parameters(FLOAT_PARA);
		break;
	case RGB_INP:
		uitm_name = "RGB : " + name;
		uitm_para = Parameters(VEC3_PARA);
		break;
	case BUTTON_INP:
		uitm_name = "BUTTON : " + name;
		break;
	case IMAGE_INP:
		uitm_name = "IMAGE : " + name;
		break;
	case BOOL_INP:
		uitm_name = "BOOL : " + name;
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
