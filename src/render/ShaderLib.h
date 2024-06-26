#pragma once

#include <GL/glew.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>

#include "Parameters.h"

typedef std::pair<ParaType, std::string> Arg;
typedef std::vector<Arg> Args;

			//|    struct_name    +   name   +   count   |
typedef std::tuple<std::string, std::string, int> S_Struct, S_var;

			//|    loc    +    name    +   params(para_type  +  name)   |
typedef std::tuple<int, std::string, Args> S_SB, S_Struct_DEF; //storage buffer

			//|   out_type   +    name    +   content   +   args(in_type  +  name)   |
typedef std::tuple<ParaType, std::string, std::string, Args> S_func, S_const, S_UBuffer;

			//|    name    +    type    +    count    |
typedef std::tuple<std::string, ParaType, int> S_U, S_OUT, S_IN;

			//|    name    +     type    +    data    |
typedef std::tuple<std::string, ParaType, float> S_glob;

			//|    loc    +    name    +   type   |
typedef std::tuple<int, std::string, ParaType> S_AB, S_REND; //array buffer

enum ShaderType
{
	NONE_SHADER = -1, VERTEX_SHADER, FRAGMENT_SHADER, COMPUTE_SHADER, GEOMETRY_SHADER
};

enum ShaderPropType
{
	NONE_PROP = -1, LAYOUT_IN_PROP, LAYOUT_BUFFER_PROP, LAYOUT_OUT_PROP, IN_PROP, OUT_PROP, UNIFRON_PROP, STURCT_DEF_PROP, STURCT_PROP, UNIFORM_STRUCT_PROP, CONST_PROP, GLOB_PROP, FUNC_DEF_PROP, END_PROP
};

class ShaderStruct {
private:

	int _max_loc[3]{ 0,0,0 };
	std::vector<int> _LAY_LOC[3];
	bool _is_registered(int _loc, ShaderPropType _type) { for (int i : _LAY_LOC[(int)_type]) if (i == _loc)return true; return false; }
	int _get_avail_loc(int _loc, ShaderPropType _type) { while (_is_registered(_loc, _type))_loc++; _LAY_LOC[(int)_type].push_back(_loc); return _loc; }

public:
	static std::vector<std::string> type_table;
public:

	bool is_struct_changed = true;
	int version = 330;

public:

	std::vector<S_AB> AB_list;
	std::vector<S_REND> pass_list;
	std::vector<S_UBuffer> ubuffer_list;
	std::vector<S_SB> SB_list;
	std::vector<S_Struct_DEF> struct_def_list;
	std::vector<S_U> uniform_list;
	std::vector<S_Struct> uniform_struct_list;
	std::vector<S_IN> input_list;
	std::vector<S_OUT> output_list;
	std::vector<S_const> const_list;
	std::vector<S_glob> glob_list;
	std::vector<S_var> vari_list;

	std::vector<int> func_list_state;
	std::vector<S_func> func_list, buildin_func_list;

	std::string Main;

public:

	static std::string ParseType(ParaType type);
	static ParaType ParseType(const std::string& type);
	static std::string ParseCount(int count);
	static std::string ParseArgs(const Args& args);
	static Args ParseArgs(const std::string& args);

public:

	static bool IsAvailType(const std::string& type);
	bool IsIOLinked(std::string_view _name, bool _type = 0);
	static void ADD_TYPE(const std::string& name);
	static void _debug();

public:

	void SetAB			(int _loc, ParaType type, const std::string& _name)				{ is_struct_changed = true; AB_list.emplace_back	(_get_avail_loc(_loc, LAYOUT_IN_PROP)	 , _name, type);}
	void SetPass		(int _loc, ParaType type, const std::string& _name)				{ is_struct_changed = true; pass_list.emplace_back	(_get_avail_loc(_loc, LAYOUT_BUFFER_PROP), _name, type);}
	void SetSB			(int _loc, const std::string& _name, const Args& args)			{ is_struct_changed = true; SB_list.emplace_back	(_get_avail_loc(_loc, LAYOUT_OUT_PROP)	 , _name, args);}
	void SetUB			(std::string _type, std::string _var, const Args& args)			{ is_struct_changed = true; ubuffer_list.emplace_back(NONE_PARA, _type, _var, args); }
	void SetUni			(ParaType _type, int count, const std::string& _name)			{ is_struct_changed = true; uniform_list.emplace_back(_name, _type, count); }
	void SetInp			(ParaType _type, int count, const std::string& _name)			{ is_struct_changed = true; input_list.emplace_back(_name, _type, count); }
	void SetOut			(ParaType _type, int count, const std::string& _name)			{ is_struct_changed = true; output_list.emplace_back(_name, _type, count); }
	void SetGlob		(ParaType _type, float defult, const std::string& _name)		{ is_struct_changed = true; glob_list.emplace_back(_name, _type, defult); }
	void DefStruct		(const std::string& _name, const Args& args)					{ is_struct_changed = true; struct_def_list.emplace_back(0, _name, args); }
	void DefFunc		(ParaType _type, std::string _name, const std::string& content, const Args& args) { is_struct_changed = true; func_list.emplace_back(_type, _name, content, args); }
	void SetBuildinF    (const S_func& buildin)											{ is_struct_changed = true; buildin_func_list.emplace_back(buildin); }
	void SetBuildinC	(const S_const& buildin)										{ is_struct_changed = true; const_list.emplace_back(buildin); }
	void SetConst		(ParaType _type, const std::string& _name, const std::string& content) { is_struct_changed = true; const_list.emplace_back(_type, _name, content, NULL); }
	void SetVar			(const std::string& _type, const std::string& _name, int count)	{ is_struct_changed = true; vari_list.emplace_back(_type, _name, count); }

public:

	std::string GenerateShader();
	void Reset();

};

class ShaderLib
{
public:

	ShaderLib() {};
	~ShaderLib() {};

public:

	static std::vector<std::string> SSR_prefix;
	static std::vector<std::string> AA_prefix;
	static std::vector<std::string> AO_prefix;
	static std::vector<std::string> Shadow_prefix;

public:
	// Function Lib
	const static S_func Gamma;
	const static S_func FilmicF;
	const static S_func FilmicV4;
public:
	// Struct Lib
public:
	// Const Lib
	const static S_const PI;
	const static S_const Pix_UV_ratio;

	const static S_const v_albedo;
	const static S_const v_metal;
	const static S_const v_rough;
	const static S_const v_specu;
	const static S_const v_emis_c;
	const static S_const v_emis_s;
	const static S_const v_alpha;

	const static S_const t_albedo;
	const static S_const t_metal;
	const static S_const t_rough;
	const static S_const t_specu;
	const static S_const t_emis_c;
	const static S_const t_emis_s;
	const static S_const t_alpha;

	const static std::vector<S_const> v_material;
	const static std::vector<S_const> t_material;
};
