#pragma once
#include "Parameters.h"
#include <iostream>
#include <optional>
enum ImItemType
{
	NONE_ITEM, TEXT, FLOAT_INP, RGB_INP, BUTTON
};

class ImguiItem
{
public:
	ImguiItem();
	ImguiItem(ImItemType type);
	~ImguiItem();

	template<typename T>
	void SetItemDefulValue(T deful);

	ImItemType uitm_type = NONE_ITEM;
	std::string uitm_name = "";
	std::optional<Parameters> uitm_para;
};

template<typename T>
void ImguiItem::SetItemDefulValue(T deful)
{

}

