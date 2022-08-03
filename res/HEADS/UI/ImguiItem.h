#pragma once
#include "Parameters.h"
#include "ImGui/imgui.h"
#include "support.h"

#include <iostream>
#include <optional>
#include <type_traits>
enum ImItemType
{
	NONE_INP, TEXT_INP, FLOAT_INP, RGB_INP, RGBA_INP, BUTTON_INP, IMAGE_INP, BOOL_INP
};

class ImguiItem
{
public:
	ImguiItem();
	ImguiItem(ImItemType type, const std::string& name);
	ImguiItem(const Parameters& para);
	~ImguiItem();

	template<typename T>
	void SetItemDefulValue(T deful);

	ImItemType uitm_type = NONE_INP;
	std::string uitm_name = "";
	Parameters uitm_para;

	void Rename(const std::string& name) { uitm_name = name; }
};

template<typename T>
void ImguiItem::SetItemDefulValue(T deful)
{

	if (std::is_same<T,int>)
	{
		uitm_para->para_data.idata = deful;

	}else if (std::is_same<T, bool>)
	{
		uitm_para->para_data.bdata = deful;

	}
	else if (std::is_same<T, float>)
	{
		uitm_para->para_data.fdata = deful;

	}
	else if (std::is_same<T, std::string>)
	{
		uitm_para->para_data.sdata = deful;

	}
	else if (std::is_same<T, glm::vec2>)
	{
		uitm_para->para_data.v2data = deful;

	}
	else if (std::is_same<T, glm::vec3>)
	{
		uitm_para->para_data.v3data = deful;

	}
	else if (std::is_same<T, glm::vec4>)
	{
		uitm_para->para_data.v4data = deful;

	}
}

