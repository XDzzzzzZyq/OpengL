#include "ParaInput.h"

UI::ParaInput::ParaInput()
{

}

UI::ParaInput::ParaInput(ImItemType type, const std::string& name)
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
	case BOOL_INP:

		uitm_para = Parameters(BOOL_PARA);
		break;
	default:
		break;
	}
}

UI::ParaInput::ParaInput(const Parameters& para)
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

UI::ParaInput::ParaInput(ImItemType type, const std::string& name, float min, float max,float def)
{
	uitm_type = type;
	notagname = uitm_name = name;

	switch (type)
	{
	case NONE_INP:
		uitm_name = "NONE";
		break;
	case FLOAT_INP:
		uitm_para = Parameters(FLOAT_PARA);
		uitm_para.para_data.fdata = def;
		break;
	case BOOL_INP:
		uitm_para = Parameters(BOOL_PARA);
		uitm_para.para_data.bdata = def;
		break;
	case INT_INP:
		uitm_para = Parameters(INT_PARA);
		uitm_para.para_data.idata = def;
	default:
		break;
	}
	SetRange(min, max);
}

void UI::ParaInput::RenderItem() const
{
	//DEBUG("render "+uitm_name)
	switch (uitm_type)
	{
	case NONE_INP:
		break;
	case FLOAT_INP:
		ImGui::SliderFloat(

			GetCharName(),
			&uitm_para.para_data.fdata,
			uitm_para.para_data.data_range[0],
			uitm_para.para_data.data_range[1]

		);

		break;
	case INT_INP:
		ImGui::SliderInt(

			GetCharName(),
			&uitm_para.para_data.idata,
			uitm_para.para_data.data_range[0],
			uitm_para.para_data.data_range[1]

		);

		break;
	case RGB_INP:
		ImGui::ColorEdit3(

			GetCharName(),
			uitm_para.para_data.v3data

		);

		break;
	case RGBA_INP:
		ImGui::ColorEdit4(

			GetCharName(),
			uitm_para.para_data.v4data

		);

		break;
	case BOOL_INP:
		uitm_para.para_data.bdata = ImGui::RadioButton(
			
			GetCharName(), 
			false
		
		);
		break;
	default:
		break;
	}
}

Parameters* UI::ParaInput::GetPara()
{
	return &uitm_para;
}
