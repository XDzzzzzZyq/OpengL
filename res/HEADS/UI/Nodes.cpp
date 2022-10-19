#include "Nodes.h"

Nodes::Nodes()
	:Nodes(NONE_NODE)
{

}

Nodes::Nodes(NodeType _type)
	:Nodes("[Unknown]", _type)
{

}

Nodes::Nodes(const char* _name, NodeType _type)
	:n_name(_name), n_type(_type)
{

}
