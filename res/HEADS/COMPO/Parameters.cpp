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
		

		break;
	case VEC3_PARA:
		

		break;
	case VEC4_PARA:
		

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
