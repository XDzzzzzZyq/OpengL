#pragma once

#include <GL/glew.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "structs.h"
#include "Parameters.h"

typedef std::pair<ParaType, std::string> Arg;
typedef std::vector<Arg> Args;

			//|    struct_name    +   name   +   count   |
typedef std::tuple<std::string, std::string, int> S_Struct, S_var;

			//|    loc    +    name    +   params(para_type  +  name)   |
typedef std::tuple<int, std::string, Args> S_SB, S_Struct_DEF; //storage buffer

			//|   out_type   +    name    +   content   +   args(in_type  +  name)   |
typedef std::tuple<ParaType, std::string, std::string, Args> S_func, S_const; 

			//|    name    +    type    +    count    |
typedef std::tuple<std::string, ParaType, int> S_U, S_OUT, S_IN;

			//|    name    +     type    +    data    |
typedef std::tuple<std::string, ParaType, float> S_glob;

			//|    loc    +    name    +   type   |
typedef std::tuple<int, std::string, ParaType> S_AB, S_REND; //array buffer

enum ShaderType
{
	NONE_SHADER=-1, VERTEX_SHADER, FRAGMENT_SHADER, COMPUTE_SHADER
};

struct ShaderStruct {
private:
	static std::vector<std::string> type_table;
public:
	bool is_struct_changed = true;
	int version = 330;
	ShaderType type = NONE_SHADER;

	std::vector<S_AB> AB_list;
	std::vector<S_REND> pass_list;
	std::vector<S_SB> SB_list;
	std::vector<S_Struct_DEF> struct_def_list;
	std::vector<S_U> uniform_list;
	std::vector<S_Struct> uniform_struct_list;
	std::vector<S_IN> input_list;
	std::vector<S_OUT> output_list;
	std::vector<S_const> const_list;
	std::vector<S_glob> glob_list;
	std::vector<S_var> vari_list;

	std::vector<S_func> func_list;

	std::string Main;

	static std::string ParseType(ParaType type);
	static ParaType ParseType(const std::string& type);
	static std::string ParseCount(int count);
	static std::string ParseArgs(const Args& args);
	static Args ParseArgs(const std::string& args);
	static bool IsAvailType(const std::string& type);
	static void ADD_TYPE(const std::string& name);
	static void Debug() {for(auto& i : type_table)DEBUG("|"+i+"|") }
	void SetAB			(int loc, ParaType type, const std::string& name)				{ is_struct_changed = true; AB_list.emplace_back(loc, name, type); }
	void SetPass		(int loc, ParaType type, const std::string& name)				{ is_struct_changed = true; pass_list.emplace_back(loc, name, type); }
	void SetSB			(int loc, const std::string& name, const Args& args)			{ is_struct_changed = true; SB_list.emplace_back(loc, name, args); }
	void SetUni			(ParaType type, int count, const std::string& name)				{ is_struct_changed = true; uniform_list.emplace_back(name, type, count); }
	void SetInp			(ParaType type, int count, const std::string& name)				{ is_struct_changed = true; input_list.emplace_back(name, type, count); }
	void SetOut			(ParaType type, int count, const std::string& name)				{ is_struct_changed = true; output_list.emplace_back(name, type, count); }
	void SetGlob		(ParaType type, float defult, const std::string& name)			{ is_struct_changed = true; glob_list.emplace_back(name, type, defult); }
	void DefStruct		(const std::string& name, const Args& args)						{ is_struct_changed = true; struct_def_list.emplace_back(0, name, args); }
	void DefFunc		(ParaType type, const std::string& name, const std::string& content, const Args& args) { is_struct_changed = true; func_list.emplace_back(type, name, content, args); }
	void SetConst		(ParaType type, const std::string& name, const std::string& content) { is_struct_changed = true; const_list.emplace_back(type, name, content, NULL); }
	void SetVar			(const std::string& type, const std::string& name, int count)	{ is_struct_changed = true; vari_list.emplace_back(type, name, count); }
};

class ShaderLib
{
private:

public:
	ShaderLib();
	~ShaderLib();

	void InitLib();
public:
	ShaderType active_shader;
	ShaderStruct shader_struct_list[2];
	std::string shader_list[2];
	std::string GenerateShader(ShaderType tar = NONE_SHADER);
	void ShaderLibDebug() { DEBUG("[Vert Shader]\n" + shader_list[FRAGMENT_SHADER]) };
public:
	// Function Lib
	static S_func gamma;
public:
	static S_const PI;
};
