#pragma once
#include "support.h"

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
	mutable float fdata=0.0f;
	mutable int idata=0;
	mutable std::string sdata="";
	mutable bool bdata=false;
	mutable float v2data[2] = { 0.0f,0.0f };
	mutable float v3data[3] = {0.0f,0.0f,0.0f };
	mutable float v4data[4] = {0.0f,0.0f,0.0f ,0.0f };
	mutable float data_range[2] = { -10.0f, 10.0f };
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

