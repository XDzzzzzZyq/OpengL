#include "Parameters.h"

Parameters::Parameters()
{

}

Parameters::Parameters(ParaType _type, const std::string& _name)
	:para_type(_type), para_name(_name), para_data(ParaData())
{
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
