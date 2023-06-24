#include "ShaderLib.h"

std::vector<std::string> ShaderStruct::type_table = { "none", "float", "int", "bool", "vec2", "vec3", "vec4", "mat3", "mat4", "sampler2D", "samplerCube"};
std::string ShaderLib::folder_root = "res/shaders/";;
std::vector<std::string> ShaderLib::file_type = { ".vert", ".frag", ".comp", ".geom"};

ShaderLib::ShaderLib()
{
	//InitLib();
}

ShaderLib::~ShaderLib()
{

}

void ShaderLib::InitLib()
{
	shader_struct_list[VERTEX_SHADER].SetAB(0, VEC3_PARA, "position");
	shader_struct_list[VERTEX_SHADER].SetAB(1, VEC2_PARA, "uvIn");
	shader_struct_list[VERTEX_SHADER].SetAB(2, VEC3_PARA, "normal");
	shader_struct_list[VERTEX_SHADER].SetAB(3, VEC3_PARA, "smo_normal");

	shader_struct_list[VERTEX_SHADER].SetOut(VEC2_PARA, 1, "uv");
	shader_struct_list[VERTEX_SHADER].SetOut(VEC4_PARA, 1, "testcolor");
	shader_struct_list[VERTEX_SHADER].SetOut(VEC3_PARA, 1, "pix_pos");
	shader_struct_list[VERTEX_SHADER].SetOut(VEC4_PARA, 1, "normal_color");
	shader_struct_list[VERTEX_SHADER].SetOut(VEC4_PARA, 1, "Snormal_color");

	shader_struct_list[VERTEX_SHADER].SetUni(MAT4_PARA, 1, "cam_trans");
	shader_struct_list[VERTEX_SHADER].SetUni(MAT4_PARA, 1, "obj_trans");
	shader_struct_list[VERTEX_SHADER].SetUni(MAT4_PARA, 1, "ProjectM");
	shader_struct_list[VERTEX_SHADER].SetUni(FLOAT_PARA, 5, "z_inp");

	shader_struct_list[VERTEX_SHADER].SetPass(0, VEC4_PARA, "color");
	shader_struct_list[VERTEX_SHADER].SetPass(1, VEC4_PARA, "IDcolor");
	shader_struct_list[VERTEX_SHADER].SetPass(2, VEC4_PARA, "RANDcolor");
	shader_struct_list[VERTEX_SHADER].SetPass(3, VEC4_PARA, "SELECcolor");

	shader_struct_list[VERTEX_SHADER].SetGlob(FLOAT_PARA, 3.1415926f, "pi");
	shader_struct_list[VERTEX_SHADER].SetGlob(VEC4_PARA, 1.0f, "const_color");

	shader_struct_list[VERTEX_SHADER].DefStruct(
		"spot_light",
		{ {VEC3_PARA, "pos"}, {VEC3_PARA, "color"}, {FLOAT_PARA, "power"} }
	);

	shader_struct_list[VERTEX_SHADER].DefFunc(
		VEC3_PARA,
		"Filmic",
		"\treturn x * inp;\n\n",
		{ {FLOAT_PARA, "x"},{VEC3_PARA, "inp"} }
	);
}

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
	return NONE_PARA;
}

std::string ShaderStruct::ParseCount(int count)
{
	return count > 1 ? "["+ std::to_string(count) +"]" : "";
}

std::string ShaderStruct::ParseArgs(const Args& args)
{
	std::string result="(";

	if (args.size()) {
		for (const auto i : args)
			result += ParseType(i.first) + " " + i.second + ", ";
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
