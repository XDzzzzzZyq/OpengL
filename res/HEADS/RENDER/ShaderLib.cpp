#include "ShaderLib.h"
#include "macros.h"

std::vector<std::string> ShaderStruct::type_table = { "float", "int", "bool", "none", "vec2", "vec3", "vec4", "mat3", "mat4", "sampler2D", "samplerCube"};

std::string ShaderStruct::ParseType(ParaType type)
{
	return type_table[type];
}

ParaType ShaderStruct::ParseType(const std::string& type)
{
	LOOP(type_table.size()) {
		if (type_table[i] == type)
			return (ParaType)i;
	}

	ADD_TYPE(type);
	return ParseType(type);
}

std::string ShaderStruct::ParseCount(int count)
{
	return count > 1 ? "["+ std::to_string(count) +"]" : "";
}

std::string ShaderStruct::ParseArgs(const Args& args)
{
	std::string result="(";

	if (args.size()) {
		for (const auto& [type, name] : args)
			result += ParseType(type) + " " + name + ", ";
		result.erase(result.end() - 2, result.end() - 1);
	}
	return result+")";
}

Args ShaderStruct::ParseArgs(const std::string& args)
{
	Args result;
	std::istringstream str(args);
	std::string word;
	while (str>>word)
	{
		ParaType type = ParseType(word);
		str >> word;
		result.emplace_back(type, word.erase(word.size() - 1, 1));
	}
	return result;
}

bool ShaderStruct::IsAvailType(const std::string& type)
{

	for (const auto& i : type_table)
		if (type == i)
			return true;
	return false;
}

bool ShaderStruct::IsIOLinked(std::string_view _name, bool _type /*= 0*/)
{
	for (auto& i : _type ? output_list : input_list)
	{
		if (_name == std::get<0>(i))
			return true;
	}

	return false;
}

void ShaderStruct::ADD_TYPE(const std::string& name)
{
	if(std::find(type_table.begin() + TEXTURE_PARA, type_table.end(), name)==type_table.end())
		type_table.emplace_back(name);
}

void ShaderStruct::Reset()
{
	AB_list = {};
	pass_list = {};
	SB_list = {};
	struct_def_list = {};
	uniform_list = {};
	uniform_struct_list = {};
	input_list = {};
	output_list = {};
	const_list = {};
	glob_list = {};
	vari_list = {};

	func_list_state = {};
	func_list = {};
	buildin_func_list = {};

	Main = "";

	LOOP(3) {
		_max_loc[i] = 0;
		_LAY_LOC[i] = {};
	}

}
