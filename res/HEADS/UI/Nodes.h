#pragma once

#include "support.h"


#include "Parameters.h"
#include "Transform.h"

enum NodeType
{
	NONE_NODE = -1, SCL_MATH_NODE, VEC_MATH_NODE
};

class Nodes : public Transform2D
{
	//			|	Nodes   |   Index=-1   |
	typedef std::pair<Nodes*, int> ParaLink;

public:
	NodeType n_type;
	std::string n_name;
	std::string n_label;


	ImU32 n_color;
public:
	std::vector<Parameters> n_in;
	std::vector<Parameters> n_out;

	std::vector<ParaLink> n_in_link;     // ->[in   out]
	std::vector<ParaLink> n_out_link;    //   [in   out]->
public:
	Nodes();
	Nodes(NodeType _type);
	Nodes(const char* _name, NodeType _type);
};

