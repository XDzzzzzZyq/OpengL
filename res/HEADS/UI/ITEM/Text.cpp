#include "Text.h"

UI::Text::Text()
{
	uitm_type = TEXT_OUTP;
}


UI::Text::Text(const std::string& name)
{
	notagname = uitm_name = name;
	uitm_type = TEXT_OUTP;
	
}


void UI::Text::RenderItem() const
{

	if (mode)
	{
		switch (args_count)
		{
		case 0:		ImGui::Text(GetCharName());break;
		case 1:		ImGui::Text(GetCharName(), args_ptr[0]);break;
		case 2:		ImGui::Text(GetCharName(), args_ptr[0], args_ptr[1]);break;
		case 3:		ImGui::Text(GetCharName(), args_ptr[0], args_ptr[1], args_ptr[2]);break;
		case 4:		ImGui::Text(GetCharName(), args_ptr[0], args_ptr[1], args_ptr[2], args_ptr[3]);break;
		case 5:		ImGui::Text(GetCharName(), args_ptr[0], args_ptr[1], args_ptr[2], args_ptr[3], args_ptr[4]);break;
		default:	break;
		}
	}
	else
	{
		switch (args_count)
		{
		case 0:		ImGui::Text(GetCharName());break;
		case 1:		ImGui::Text(GetCharName(), args_list[0]);break;
		case 2:		ImGui::Text(GetCharName(), args_list[0], args_list[1]);break;
		case 3:		ImGui::Text(GetCharName(), args_list[0], args_list[1], args_list[2]);break;
		case 4:		ImGui::Text(GetCharName(), args_list[0], args_list[1], args_list[2], args_list[3]);break;
		case 5:		ImGui::Text(GetCharName(), args_list[0], args_list[1], args_list[2], args_list[3], args_list[4]);break;
		default:	break;
		}
	}
}
