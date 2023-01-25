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
	enum NodePinType { OUT = 0, IN = 1 };
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
	[[nodiscard]] static Parameters* GetParamPtr(Nodes* _tar,int _idx, bool _type) { return _type ? &_tar->n_in[_idx] : &_tar->n_out[_idx]; }  // true: n_in  | false: n_out
	[[nodiscard]] static Parameters* GetParamPtr(const ParaLink& _link, bool _type) { return GetParamPtr(_link.first, _link.second, _type); }  // true: n_in  | false: n_out
	[[nodiscard]] static int GetParamLoc(Nodes* _tar, Parameters* _param, bool _type);  // return the location of the parameter in the target node

	void PushIn (const Parameters& _in);
	void PushOut(const Parameters& _out);
	void LinkIn (int _self_idx, Nodes* _tar, int _idx);
	void LinkOut(int _self_idx, Nodes* _tar, int _idx);
	static void BreakLink(Parameters* _param, bool _type);
	static bool IsLinked(Parameters* _param, bool _type) {
		return _type ? n_in_link.find(_param) != n_in_link.end() : n_out_link.find(_param) != n_out_link.end();
	}
public:
	Nodes();
	Nodes(NodeType _type);
	Nodes(const char* _name, NodeType _type);
};

