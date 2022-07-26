#pragma once

#include "ImguiItem.h"
namespace UI {
	class ParaInput : public ImguiItem
	{
	public:
		ParaInput();
		ParaInput(ImItemType type, const std::string& name);
		ParaInput(ImItemType type, const std::string& name, const glm::vec3& _def);
		ParaInput(ImItemType type, const std::string& name, const glm::vec4& _def);
		//template<typename T>
		//ParaInput(ImItemType type, const std::string& name, const T& _def);
		ParaInput(const Parameters& para);
		ParaInput(ImItemType type, const std::string& name, float min, float max, float def=0.0f);

		Parameters uitm_para;
	public:
		void SetType(ImItemType _type);
		void SetType(ParaType _type);
		template<typename T>
		void SetDefulValue(const T& _def);
		void SetRange(float min, float max) const {
			uitm_para.para_data.data_range[0] = min;
			uitm_para.para_data.data_range[1] = max;
		}

		void RenderItem() const override;
		Parameters* GetPara() override;
	};
}


template<typename T>
void UI::ParaInput::SetDefulValue(const T& _def)
{

	if (std::is_same<T, int>)
	{
		uitm_para.para_data.idata = _def;

	}
	else if (std::is_same<T, bool>)
	{
		uitm_para.para_data.bdata = _def;

	}
	else if (std::is_same<T, float>)
	{
		uitm_para.para_data.fdata = _def;

	}
	else if (std::is_same<T, std::string>)
	{
		uitm_para.para_data.sdata = _def;

	}
	else if (std::is_same<T, glm::vec2>)
	{
		uitm_para.para_data.v2data = _def;

	}
	else if (std::is_same<T, glm::vec3>)
	{
		uitm_para.para_data.v3data = _def;

	}
	else if (std::is_same<T, glm::vec4>)
	{
		uitm_para.para_data.v4data = _def;

	}
}

//#pragma once
//template<typename T>
//UI::ParaInput::ParaInput(ImItemType type, const std::string& name, const T& _def)
//	:ParaInput(type, name)
//{
//	SetDefulValue(_def);
//}