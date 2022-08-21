#include "ImguiTheme.h"

ImguiTheme::ImguiTheme()
{
	th_name = "Default Theme";

	InitParaLoc();
}

ImguiTheme::ImguiTheme(const std::string& name)
{
	th_name = name;

	InitParaLoc();
}

ImguiTheme::~ImguiTheme()
{
}

ThemeData ImguiTheme::th_data;

float ImguiTheme::GetFloatData(const std::string& th_para_name) const
{
	return th_data.float_data[name_loc_buffer[th_para_name]];
}

ImVec2 ImguiTheme::GetVec2Data(const std::string& th_para_name) const
{
	return th_data.vec2_data[name_loc_buffer[th_para_name]];
}

ImU32 ImguiTheme::GetColorData(const std::string& th_para_name) const
{
	return th_data.color_data[name_loc_buffer[th_para_name]];
}

void ImguiTheme::SetFloatData(const std::string& th_para_name, float para) const
{
	th_data.float_data[name_loc_buffer[th_para_name]] = para;
}

void ImguiTheme::SetVec2Data(const std::string& th_para_name, const ImVec2& para) const
{
	th_data.vec2_data[name_loc_buffer[th_para_name]] = para;
}

void ImguiTheme::SetColorData(const std::string& th_para_name, const ImU32& para) const
{
	th_data.color_data[name_loc_buffer[th_para_name]] = para;
}

void ImguiTheme::InitParaLoc()
{
	PushFloatData("Outline opac", 0.2);
}

void ImguiTheme::PushFloatData(const std::string& name, float data /*= 0.0f*/)
{
	th_data.float_data.push_back(data);
	name_loc_buffer[name] = th_data.float_data.size() - 1;
}

void ImguiTheme::PushVec2Data(const std::string& name, const ImVec2& data /*= ImVec2(0,0)*/)
{
	th_data.vec2_data.push_back(data);
	name_loc_buffer[name] = th_data.vec2_data.size() - 1;
}

void ImguiTheme::PushColorData(const std::string& name, const ImU32& data /*= IM_COL32(0, 0, 0, 0)*/)
{
	th_data.color_data.push_back(data);
	name_loc_buffer[name] = th_data.color_data.size() - 1;
}
