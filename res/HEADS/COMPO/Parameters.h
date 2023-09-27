#pragma once

#include "glm/glm.hpp"

#include <unordered_map>
#include <variant>

enum ParaType
{
	NONE_PARA=-1, FLOAT_PARA, INT_PARA, BOOL_PARA, STRING_PARA, VEC2_PARA, VEC3_PARA, VEC4_PARA, MAT3_PARA, MAT4_PARA, TEXTURE_PARA, CUSTOM_PARA
};

struct ParaData
{
	ParaData() {}
	ParaData(const ParaData& copy);
	~ParaData() {}
	float fdata{ 0.0f };
	int idata{ 0 };
	std::string sdata{ "" };
	bool bdata{ false };
	glm::vec2 v2data{ 0.0f };
	glm::vec3 v3data{ 0.0f };
	glm::vec4 v4data{ 0.0f };
	glm::vec2 data_range{ -10.0f, 10.0f };
};

class Parameters
{
public:
	Parameters();
	Parameters(ParaType _type, const std::string& _name = "unknown param");
	Parameters(const Parameters& para); //copy
	~Parameters();

	bool is_para_changed;
	unsigned int para_uniformID = 0;
	std::string para_name;
	ParaType para_type;
	ParaData para_data;

	Parameters GenParaItem(ParaType type);
	ParaData* GetParaPtr() { return &para_data; }
	void Rename(const std::string& name) { para_name = name; }
};

