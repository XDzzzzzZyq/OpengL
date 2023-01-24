#pragma once
#include "ParaInput.h"

UI::ParaInput::ParaInput()
{

}


UI::ParaInput::ParaInput(ImItemType type, const std::string& name)
{
	uitm_type = type;
	notagname = uitm_name = name;
	SetType(type);
}

UI::ParaInput::ParaInput(ImItemType type, const std::string& name, const glm::vec4& _def)
	:ParaInput(type, name)
{
	//SetDefulValue(_def);
	LOOP(4)
		uitm_para.para_data.v4data[i] = _def[i];
}

UI::ParaInput::ParaInput(ImItemType type, const std::string& name, const glm::vec3& _def)
	: ParaInput(type, name)
{
	//SetDefulValue(_def);
	LOOP(3)
		uitm_para.para_data.v3data[i] = _def[i];
}

UI::ParaInput::ParaInput(const Parameters& para)
{
	uitm_para = para;
	uitm_name = para.para_name;
	SetType(para.para_type);
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
	is_value_changed = RenderParam(&uitm_para, GetCharName(), GetCharName(), uitm_type, false);
	
}

bool UI::ParaInput::RenderParam(const Parameters* _param, const char* _ID, ImItemType _type, float _size)
{
	return RenderParam(_param, _param->para_name.c_str(), _ID, _type);
}

bool UI::ParaInput::RenderParam(const Parameters* _param, const char* _ID, ImItemType _type /*= FLOAT_INP*/, float _size /*= 1.0f*/, float _length /*= 10.0f*/)
{
	return RenderParam(_param, _param->para_name.c_str(), _ID, _type, true, _size, _length);
}

bool UI::ParaInput::RenderParam(
	const Parameters* _param, 
	const char* _name, 
	const char* _ID, 
	ImItemType _type /*= FLOAT_INP*/, 
	bool _is_movable,
	float _size /*= 1.0f*/, 
	float _length /*= 10.0f*/)
{
	switch (_type)
	{
	case NONE_INP:
		break;
	case FLOAT_INP:
		return ImGui::SliderFloat(

			_name,
			&_param->para_data.fdata,
			_param->para_data.data_range[0],
			_param->para_data.data_range[1],
			NULL,
			0,
			_ID,
			_is_movable,
			_size,
			_length
		);
	case INT_INP:
		return ImGui::SliderInt(

			_name,
			&_param->para_data.idata,
			_param->para_data.data_range[0],
			_param->para_data.data_range[1]

		);
	case RGB_INP:
		return ImGui::ColorEdit3(

			_name,
			_param->para_data.v3data

		);
	case RGBA_INP:
		return ImGui::ColorEdit4(

			_name,
			_param->para_data.v4data

		);
	case VEC2_INP:
		return ImGui::InputFloat2(

			_name,
			_param->para_data.v2data

		);
	case VEC3_INP:
		return ImGui::InputFloat3(

			_name,
			_param->para_data.v3data

		);
	case VEC4_INP:
		return ImGui::InputFloat4(

			_name,
			_param->para_data.v4data

		);
	case BOOL_INP:
		_param->para_data.bdata = ImGui::RadioButton(

			_name,
			false

		);
		return true;
	default:
		break;
	}
}

void UI::ParaInput::SetType(ImItemType _type)
{

	switch (_type)
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
	case RGBA_INP:

		uitm_para = Parameters(VEC4_PARA);
		break;
	case BOOL_INP:

		uitm_para = Parameters(BOOL_PARA);
		break;
	default:
		break;
	}
}
void UI::ParaInput::SetType(ParaType _type)
{
	switch (_type)
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

Parameters* UI::ParaInput::GetPara()
{
	return &uitm_para;
}
