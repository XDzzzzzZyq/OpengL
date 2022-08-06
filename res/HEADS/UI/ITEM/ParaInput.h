#pragma once

#include "ImguiItem.h"
namespace UI {
	class ParaInput : public ImguiItem
	{
	public:
		ParaInput();
		ParaInput(ImItemType type, const std::string& name);
		ParaInput(const Parameters& para);
		ParaInput(ImItemType type, const std::string& name, float min, float max, float def=0.0f);

		Parameters uitm_para;

		template<typename T>
		void SetItemDefulValue(T deful);

		void SetRange(float min, float max) const {
			uitm_para.para_data.data_range[0] = min;
			uitm_para.para_data.data_range[1] = max;
		}

		void RenderItem() const override;
		Parameters* GetPara() override;
	};
}




template<typename T>
void UI::ParaInput::SetItemDefulValue(T deful)
{

	if (std::is_same<T, int>)
	{
		uitm_para->para_data.idata = deful;

	}
	else if (std::is_same<T, bool>)
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