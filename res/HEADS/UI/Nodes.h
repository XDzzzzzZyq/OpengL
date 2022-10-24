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
public:
	//			|	Nodes   |   Index=-1   |
	typedef std::pair<Nodes*, int> ParaLink;
private:
	static unsigned int id_buff;
public:
	static std::vector<ImU32> n_color_list;
	static std::vector<ImU32> pin_color_list;
public:
	bool is_open{ true };
	bool is_select{ false };
	bool is_activated{ false };

	unsigned int n_id;

	NodeType n_type;
	std::string n_name;
	std::string n_label;

	ImU32 n_color;    //ONLY for custom nodes
public:
	std::vector<Parameters> n_in;
	std::vector<Parameters> n_out;

	static std::unordered_map<Parameters*, ParaLink> n_in_link;     // ->[in   out]			  //Store globle Data
	static std::unordered_map<Parameters*, ParaLink> n_out_link;    //   [in   out]->		  //Store globle Data
	static Parameters* GetParamPtr(Nodes* _tar,int _idx, bool _type) { return _type ? &_tar->n_in[_idx] : &_tar->n_out[_idx]; }
	static Parameters* GetParamPtr(const ParaLink& _link, bool _type) { return GetParamPtr(_link.first, _link.second, _type); }

	void PushIn (const Parameters& _in);
	void PushOut(const Parameters& _out);
	void LinkIn (int _self_idx, Nodes* _tar, int _idx);
	void LinkOut(int _self_idx, Nodes* _tar, int _idx);
public:
	Nodes();
	Nodes(NodeType _type);
	Nodes(const char* _name, NodeType _type);
};

