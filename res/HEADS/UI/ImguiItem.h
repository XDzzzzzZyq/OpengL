#pragma once
#include "Parameters.h"
#include "ImGui/imgui.h"
#include "support.h"

#include <iostream>
#include <optional>
#include <functional>
#include <type_traits>

#define ACTIVE "ACTIVE LAYER"

enum ImItemType
{
	NONE_INP, TEXT_INP, FLOAT_INP, RGB_INP, RGBA_INP, BUTTON_INP, IMAGE_INP, BOOL_INP
};

class ImguiItem
{
private:
	mutable std::string notagname;
public:
	ImguiItem();
	ImguiItem(ImItemType type, const std::string& name);
	ImguiItem(const Parameters& para);
	~ImguiItem();

	template<typename T>
	void SetItemDefulValue(T deful);

	mutable bool using_size = false;
	mutable bool fixed_size = false;
	mutable bool show_tag_name = false;
	ImVec2 uitm_size;

	ImItemType uitm_type = NONE_INP;
	mutable std::string uitm_name = "";
	Parameters uitm_para;

	mutable std::function<void(void)> ButtonFunc = [] {};
	void CallButtonFunc() { ButtonFunc(); }


	void Rename(const std::string& name) const;
	std::string GetTagName()const;
	void EnableTagName() const ;
	void DisableTagName() const;
	const char* GetCharName() const { return uitm_name.c_str(); }
	void SetRange(float min, float max) const { uitm_para.para_data.data_range[0] = min;
												uitm_para.para_data.data_range[1] = max;	}

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

