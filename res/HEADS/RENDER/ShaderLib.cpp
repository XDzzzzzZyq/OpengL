#include "ShaderLib.h"

ShaderLib::ShaderLib()
{
	InitLib();
}

ShaderLib::~ShaderLib()
{

}

void ShaderLib::InitLib()
{
	V.SetAB(0, VEC3_PARA, "position");
	V.SetAB(1, VEC2_PARA, "uvIn");
	V.SetAB(2, VEC3_PARA, "normal");
	V.SetAB(3, VEC3_PARA, "smo_normal");

	V.SetOut(VEC2_PARA, 1, "uv");
	V.SetOut(VEC4_PARA, 1, "testcolor");
	V.SetOut(VEC3_PARA, 1, "pix_pos");
	V.SetOut(VEC4_PARA, 1, "normal_color");
	V.SetOut(VEC4_PARA, 1, "Snormal_color");
					    1,
	V.SetUni(MAT4_PARA, 1, "cam_trans");
	V.SetUni(MAT4_PARA, 1, "obj_trans");
	V.SetUni(MAT4_PARA, 1, "ProjectM");
	V.SetUni(FLOAT_PARA, 5, "z_inp");

	V.SetPass(0, VEC4_PARA, "color");
	V.SetPass(1, VEC4_PARA, "IDcolor");
	V.SetPass(2, VEC4_PARA, "RANDcolor");
	V.SetPass(3, VEC4_PARA, "SELECcolor");

	V.SetGlob(FLOAT_PARA, 3.1415926f, "pi");
	V.SetGlob(VEC4_PARA, 1.0f, "const_color");

	V.DefStruct(
		"spot_light", 
		{ {VEC3_PARA, "pos"}, {VEC3_PARA, "color"}, {FLOAT_PARA, "power"} }
	);

	V.DefFunc(
		FLOAT_PARA,
		"Filmic",
		"\treturn x * inp;\n\n",
		{ {FLOAT_PARA, "x"},{VEC3_PARA, "inp"} }
	);
}

std::string ShaderLib::GenerateShader(ShaderType tar /*= NONE_SHADER*/)
{
	if (tar == NONE_SHADER || tar == VERTEX_SHADER) {

		//
		VS += "#version " + std::to_string(V.version) + "core\n\n";

		if (V.AB_list.size()) {
			VS += "// [ARRAY_BUFFER]\n";
			for (const auto& i : V.AB_list)
				VS += "layout(location = " + std::to_string(std::get<0>(i)) + ") in " + ShaderStruct::ParseType(std::get<2>(i)) + " In_" + std::get<1>(i) + ";\n";
			VS += "\n";
		}

		if (V.pass_list.size()) {
			VS += "// [RENDER_BUFFER]\n";
			for (const auto& i : V.pass_list)
				VS += "layout(location = " + std::to_string(std::get<0>(i)) + ") out " + ShaderStruct::ParseType(std::get<2>(i)) + " Ch_" + std::get<1>(i) + ";\n";
			VS += "\n";
		}

		if (V.uniform_list.size()) {
			VS += "// [UNIFORM]\n";
			for (const auto& i : V.uniform_list)
				VS += "uniform " + ShaderStruct::ParseType(std::get<1>(i)) + "U_" + std::get<0>(i) + ShaderStruct::ParseCount(std::get<2>(i)) + ";\n";
			VS += "\n";
		}

		if (V.output_list.size()) {
			VS += "// [OUT]\n";
			for (const auto& i : V.output_list)
				VS += "out " + ShaderStruct::ParseType(std::get<1>(i)) + " Out_" + std::get<0>(i) + ShaderStruct::ParseCount(std::get<2>(i)) + ";\n";
			VS += "\n";
		}

		if (V.glob_list.size()) {
			VS += "// [GLOBES]\n";
			for (const auto& i : V.glob_list)
				VS += ShaderStruct::ParseType(std::get<1>(i)) + "Glob_" + std::get<0>(i) + " = " + ShaderStruct::ParseType(std::get<1>(i)) +"\b(" + std::to_string(std::get<2>(i)) + ");\n";
			VS += "\n";
		}

		if (V.struct_def_list.size()) {
			VS += "// [STRUCTURE_DEFINE]\n";
			for (const auto& i : V.struct_def_list) {
				VS += "struct " + std::get<0>(i) + "{\n";
				for (const auto& j : std::get<1>(i)) {
					VS += "\t" + ShaderStruct::ParseType(j.first) + j.second + ";\n";
				}
				VS += "}\n\n";
			}

			VS += "\n";
		}

		if (V.func_list.size()) {
			VS += "// [FUNCTION_DEFINE]\n";
			for (const auto& i : V.func_list) {
				VS += ShaderStruct::ParseType(std::get<0>(i)) + std::get<1>(i) + ShaderStruct::ParseArgs(std::get<3>(i)) + "{\n";
				VS += std::get<2>(i);
				VS += "}\n\n";
			}
			VS += "\n";
		}

		VS += "void main(){\n";
		VS += std::get<1>(V.Main);
		VS += "};";
	}
	if (tar == NONE_SHADER || tar == FRAGMENT_SHADER) {

	}

	return "Genertated";
}

// with space after the data type
std::string ShaderStruct::ParseType(ParaType type)
{
	switch (type)
	{
	case NONE_PARA:
		break;
	case FLOAT_PARA:
		return "float ";
		break;
	case INT_PARA:
		return "int ";
		break;
	case BOOL_PARA:
		return "bool ";
		break;
	case VEC2_PARA:
		return "vec2 ";
		break;
	case VEC3_PARA:
		return "vec3 ";
		break;
	case VEC4_PARA:
		return "vec4 ";
		break;
	case MAT3_PARA:
		return "mat3 ";
		break;
	case MAT4_PARA:
		return "mat4 ";
		break;
	case TEXTURE_PARA:
		return "sampler2D";
		break;
	default:
		break;
	}
}

std::string ShaderStruct::ParseCount(int count)
{
	return count > 1 ? "["+ std::to_string(count) +"]" : "";
}

std::string ShaderStruct::ParseArgs(Args args)
{
	std::string result="(";

	if (args.size()) {
		for (const auto i : args)
			result += ParseType(i.first) + i.second + ", ";
		result.erase(result.end() - 2, result.end() - 1);
	}
	return result+")";
}
