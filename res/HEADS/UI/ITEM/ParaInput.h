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
	public:
		mutable bool is_value_changed;
		Parameters uitm_para;
	public:
		void SetType(ImItemType _type);
		void SetType(ParaType _type);
		template<typename T>
		void SetDefulValue(const T& _def);
		void SetRange(float min, float max) {
			uitm_para.para_data.data_range[0] = min;
			uitm_para.para_data.data_range[1] = max;
		}

		void RenderItem() const override;
		static bool RenderParam(Parameters* para_param, const char* _ID, ImItemType _type = FLOAT_INP, float _size = 1.0f);			// UI panel
		static bool RenderParam(Parameters* para_param, const char* _ID, float _size, float _length);									// Node panel
		static bool RenderParam(Parameters* para_param, const char* _name, const char* _ID, ImItemType _type = FLOAT_INP, bool _is_movable = false, float _size = 1.0f, float _length = 10.0f);   //
		Parameters* GetPara() override;

	public:
		static ImItemType ParseParamType(ParaType _type);
	};
}


template<typename T>
void UI::ParaInput::SetDefulValue(const T& _def)
{
	uitm_para.Get<T>() = _def;
}

