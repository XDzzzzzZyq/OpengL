#include "Parameters.h"

Parameters::Parameters()
{

}

Parameters::Parameters(ParaType _type, const std::string& _name)
	:para_type(_type), para_name(_name), para_data(ParaData())
{
}

Parameters::~Parameters()
{

}
