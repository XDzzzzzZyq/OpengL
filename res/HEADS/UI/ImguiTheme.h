#pragma once
#include "ImGui/imgui.h"
#include <iostream>
#include <unordered_map>

struct ThemeData
{
	std::vector<float> float_data;
	std::vector<ImVec2> vec2_data;
	std::vector<ImU32> color_data;
};

class ImguiTheme
{
private:
	mutable std::unordered_map<std::string, int> name_loc_buffer;
public:
	std::string th_name = "";
	ImguiTheme();
	ImguiTheme(const std::string& name);
	~ImguiTheme();
public:
	static ThemeData th_data;
	float GetFloatData(const std::string& th_para_name) const;
	ImVec2 GetVec2Data(const std::string& th_para_name) const;
	ImU32 GetColorData(const std::string& th_para_name) const;

	void SetFloatData(const std::string& th_para_name, float para) const;
	void SetVec2Data(const std::string& th_para_name, const ImVec2& para) const;
	void SetColorData(const std::string& th_para_name, const ImU32& para) const;
public:
	void InitParaLoc();
	void PushFloatData(const std::string& name, float data = 0.0f);
	void PushVec2Data(const std::string& name, const ImVec2& data = ImVec2(0, 0));
	void PushColorData(const std::string& name, const ImU32& data = IM_COL32(0, 0, 0, 0));
};

