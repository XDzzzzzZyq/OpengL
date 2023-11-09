#pragma once

#include "glm/glm.hpp"

#include <unordered_map>
#include <variant>

enum ParaType
{
	NONE_PARA = -1,

	FLOAT_PARA,
	INT_PARA,
	BOOL_PARA,
	STRING_PARA,
	VEC2_PARA,
	VEC3_PARA,
	VEC4_PARA,

	MAT3_PARA,
	MAT4_PARA,
	TEXTURE_PARA,
	CUSTOM_PARA
};

class Parameters
{
public:

	Parameters();
	Parameters(ParaType _type, const std::string& _name = "unknown param");
	~Parameters();

public:

	struct ParaData
	{
		ParaData() {}
		~ParaData() {}
		std::variant<float, int, std::string, bool, glm::vec2, glm::vec3, glm::vec4> _data;
		glm::vec2 data_range{ -10.0f, 10.0f };
	};

	bool is_para_changed;
	std::string para_name;
	ParaType para_type;
	ParaData para_data;

	template<typename T>
	T& Get();

public:

	Parameters GenParaItem(ParaType type);
	ParaData* GetParaPtr() { return &para_data; }
	void Rename(const std::string& name) { para_name = name; }
};

template<typename T>
T& Parameters::Get()
{
	if (!std::holds_alternative<T>(para_data._data)) {
		para_data._data = T(0);
		para_type = (ParaType)para_data._data.index();
	}

	return std::get<T>(para_data._data);
}