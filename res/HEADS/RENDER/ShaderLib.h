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
typedef std::tuple<std::string, std::string, int> S_Struct;

			//|    name    +   params(para_type  +  name)   |
typedef std::tuple<std::string, Args> S_SB, S_Struct_DEF; //storage buffer

			//|   out_type   +    name    +   content   +   args(in_type  +  name)   |
typedef std::tuple<ParaType, std::string, std::string, Args> S_func; 

			//|    name    +    type    +    count    |
typedef std::tuple<std::string, ParaType, int> S_U, S_OUT, S_IN;

			//|    name    +     type    +    data    |
typedef std::tuple<std::string, ParaType, float> S_glob;

			//|    loc    +    name    +   type   |
typedef std::tuple<int, std::string, ParaType> S_AB, S_REND; //array buffer

enum ShaderType
{
	NONE_SHADER, VERTEX_SHADER, FRAGMENT_SHADER, COMPUTE_SHADER
};

struct ShaderStruct {
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
	std::vector<S_glob> glob_list;

	std::vector<S_func> func_list;

	S_func Main;

	static std::string ParseType(ParaType type);
	static std::string ParseCount(int count);
	static std::string ParseArgs(Args args);
	void SetAB(int loc, ParaType type, const std::string& name) { AB_list.emplace_back(loc, name, type); }
	void SetPass(int loc, ParaType type, const std::string& name) { pass_list.emplace_back(loc, name, type); }
	void SetUni(ParaType type, int count, const std::string& name) { uniform_list.emplace_back(name, type, count); }
	void SetInp(ParaType type, int count, const std::string& name) { input_list.emplace_back(name, type, count); }
	void SetOut(ParaType type, int count, const std::string& name) { output_list.emplace_back(name, type, count); }
	void SetGlob(ParaType type, float defult, const std::string& name) { glob_list.emplace_back(name, type, defult); }
	void DefStruct(const std::string& name, const Args& args) { struct_def_list.emplace_back(name, args); }
	void DefFunc(ParaType type, const std::string& name, const std::string& content, const Args& args) { func_list.emplace_back(type, name, content, args); }
};

class ShaderLib
{
private:
	ShaderStruct V, F;
public:
	ShaderLib();
	~ShaderLib();

	void InitLib();
public:
	std::string VS, FS;
	std::string GenerateShader(ShaderType tar = NONE_SHADER);
	void ShaderLibDebug() { GenerateShader(); DEBUG("[Vert Shader]\n" + VS) };

public:
	// Function Lib
	static S_func gamma;
};

