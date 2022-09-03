#include "Parameters.h"

Parameters::Parameters()
{

}

Parameters::Parameters(const ParaType& type)
{
	para_type = type;
	switch (type)
	{
	default:
		break;
	case NONE_PARA:
		break;
	case FLOAT_PARA:
		para_data.fdata = 0.0f;

		break;
	case INT_PARA:
		para_data.idata = 0;

		break;
	case BOOL_PARA:
		para_data.bdata = false;

		break;
	case STRING_PARA:
		para_data.sdata = "";

		break;
	case VEC2_PARA:
		para_data.v2data[0] = 0;
		para_data.v2data[1] = 0;

		break;
	case VEC3_PARA:
		para_data.v3data[0] = 0;
		para_data.v3data[1] = 0;
		para_data.v3data[2] = 0;

		break;
	case VEC4_PARA:
		para_data.v4data[0] = 0;
		para_data.v4data[1] = 0;
		para_data.v4data[2] = 0;
		para_data.v4data[3] = 0;

		break;
	}
}

Parameters::Parameters(const Parameters& para)
{
	para_type = para.para_type;
	para_data = para.para_data;
	para_name = para.para_name;
}

Parameters::~Parameters()
{

}

ParaData::ParaData(const ParaData& copy)
{
	
}
