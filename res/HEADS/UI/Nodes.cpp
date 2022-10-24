#include "Nodes.h"

unsigned int Nodes::id_buff = 1;

std::vector<ImU32> Nodes::n_color_list = { IM_COL32(0,246,83,255), IM_COL32(246,215,0,255) };
std::vector<ImU32> Nodes::pin_color_list = {};

std::unordered_map<Parameters*, Nodes::ParaLink> Nodes::n_in_link = {};
std::unordered_map<Parameters*, Nodes::ParaLink> Nodes::n_out_link = {};

void Nodes::PushIn(const Parameters& _in)
{
	n_in.push_back(_in);
}

void Nodes::PushOut(const Parameters& _out)
{
	n_out.push_back(_out);
}

void Nodes::LinkIn(int _self_idx, Nodes* _tar, int _idx)
{
	n_in_link[&n_in[_self_idx]] = std::make_pair(_tar, _idx);
}

void Nodes::LinkOut(int _self_idx, Nodes* _tar, int _idx)
{
	n_out_link[&n_out[_self_idx]] = std::make_pair(_tar, _idx);
}

Nodes::Nodes()
	:Nodes(NONE_NODE)
{

}

Nodes::Nodes(NodeType _type)
	: Nodes("[Unknown]", _type)
{

}

Nodes::Nodes(const char* _name, NodeType _type)
	: n_name(_name), n_type(_type)
{
	n_id = id_buff;
	id_buff++;
}
