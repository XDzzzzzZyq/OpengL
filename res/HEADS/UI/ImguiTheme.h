#pragma once
#include "ImGui/imgui.h"
#include <iostream>
#include <unordered_map>

//https://github.com/traverseda/OpenFontIcons
#define ICON_MIN 0xe000
#define ICON_MAX 0xe0b7

#define ICON_UNIF u8"\ue0b2"
#define ICON_PLUS u8"\ue0aa"
#define ICON_LINK u8"\ue07c"

struct ThemeData
{
	std::vector<float>				float_data;
	std::vector<ImVec2>				vec2_data;
	std::vector<ImU32>				color_data;
	std::vector<ImFont*>			font_data;
};

class ImguiTheme
{
private:
	static std::unordered_map<std::string, int> name_loc_buffer;
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
	static void PushFloatData(const std::string& name, float data = 0.0f);
	static void PushVec2Data(const std::string& name, const ImVec2& data = ImVec2(0, 0));
	static void PushColorData(const std::string& name, const ImU32& data = IM_COL32(0, 0, 0, 0));
	static void PushFontData(ImFont* font);
};

