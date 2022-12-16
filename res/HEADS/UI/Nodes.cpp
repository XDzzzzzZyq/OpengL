#include "Nodes.h"

unsigned int Nodes::id_buff = 1;

std::unordered_map<Parameters*, Nodes::ParaLink> Nodes::n_in_link = {};
std::unordered_map<Parameters*, Nodes::ParaLink> Nodes::n_out_link = {};

int Nodes::GetParamLoc(Nodes* _tar, Parameters* _param, bool _type)
{
	if (_tar == nullptr)
		return -1;

	auto& list = _type ? _tar->n_in : _tar->n_out;

	if (list.size() <= 0)
		return -1;

	return ((unsigned int)_param - (unsigned int)list.data()) / sizeof(Parameters);
}

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
	n_out_link[&_tar->n_out[_idx]] = std::make_pair(this, _self_idx);
}

void Nodes::LinkOut(int _self_idx, Nodes* _tar, int _idx)
{
	n_out_link[&n_out[_self_idx]] = std::make_pair(_tar, _idx);
	n_in_link[&_tar->n_in[_idx]] = std::make_pair(this, _self_idx);
}

void Nodes::BreakLink(Parameters* _param, bool _type)
{
	auto& map = _type ? n_in_link : n_out_link;

	bool is_linked = map.find(_param) != map.end();

	if (!is_linked) {
		DEBUG("No link")
		return;
	}

	Parameters* _tar = GetParamPtr(map[_param], !_type);
	auto& inv_map = !_type ? n_in_link : n_out_link;

	inv_map.erase(_tar);
	map.erase(_param);

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
