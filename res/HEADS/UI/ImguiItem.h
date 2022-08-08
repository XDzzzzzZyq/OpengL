#pragma once
#include "Parameters.h"
#include "ImGui/imgui.h"
#include "support.h"

#include <iostream>
#include <optional>
#include <functional>
#include <type_traits>
#include <cstdarg>



enum ImItemType
{
	NONE_INP,
	TEXT_INP, FLOAT_INP, INT_INP, RGB_INP, RGBA_INP,
	BUTTON_INP, BOOL_INP,
	IMAGE_OUTP, TEXT_OUTP,
	VIEWPORT_OUTP
};

class ImguiItem
{
private:

public:
	ImguiItem();
	ImguiItem(ImItemType type, const std::string& name);
	~ImguiItem();

	mutable bool using_size = false;
	mutable bool fixed_size = false;
	mutable bool show_tag_name = false;
	ImVec2 uitm_size;

	ImItemType uitm_type = NONE_INP;
	mutable std::string uitm_name = "";
	mutable std::string notagname = "";
	mutable int uitm_id = -1;

	mutable std::function<void(void)> ButtonFunc = [] {};

	void Rename(const std::string& name) const;
	std::string GetTagName()const;
	void EnableTagName() const ;
	void DisableTagName() const;
	const char* GetCharName() const { return uitm_name.c_str(); }

	// for all items
	virtual void RenderItem() const { DEBUG("no render function overrided")return; }

	// for all paras
	virtual Parameters* GetPara() { DEBUG(uitm_name + " is not a paraInp")return nullptr; }

	// for all buttons
	virtual void CallButtonFunc() const { DEBUG(uitm_name + " is not a button")return; }

	// for all text
	virtual void SetArgsList(float length, ...) const { DEBUG(uitm_name + " is not a text")	return; }
	virtual void SetArgsList(int length, float* f1) const { DEBUG(uitm_name + " is not a text")return; }

	// for all viewport
	virtual void ResetUV(const ImVec2& min, const ImVec2& max) { DEBUG(uitm_name + " is not a viewport")return; }
	virtual void ResetSize(const ImVec2& size) { DEBUG(uitm_name + " is not a viewport")return; }
	virtual void ResetBufferID(GLuint id) { DEBUG(uitm_name + " is not a viewport")	return; }
};
