#include "Shaders.h"
#include "ShaderLib.h"
#include "structs.h"

void RenderShader::ParseShaderFile(std::string _name, ShaderType _type) {
	Timer timer("ParseShader");

	active_shader = _type;
	shader_struct_list[active_shader].is_struct_changed = false;

	if (_name.find(ShaderLib::folder_root) == std::string::npos) 
		_name = ShaderLib::folder_root + _name + ShaderLib::file_type[_type];

	std::ifstream Stream(_name);

	ParseShaderStream(Stream, active_shader);

	shader_struct_list[active_shader].func_list_state.resize(shader_struct_list[active_shader].func_list.size());


	std::cout << "shaders are loaded up successfully!" << std::endl;
	//m_shaders = { shaders[0].str(),shaders[1].str() };
}


void RenderShader::ParseShaderStream(std::istream& _stream, ShaderType _type)
{
	std::string Line;
	std::string cache = "";
	Args args_cache;

	while (getline(_stream, Line)) {


		// PARSING
		//shaders[type] << Line << "\n";
		if (Line == "") {
		}
		else if (Line.find("#version") != std::string::npos) {
			// [Version]
			shader_struct_list[_type].version = std::atoi(Line.substr(9, 3).c_str());
		}
		else if (Line.find("layout") != std::string::npos) {
			// [layout]
			int layout = std::atoi(Line.substr(18, Line.find(")") - 18).c_str());
			if (Line.find("in ") != std::string::npos) {
				std::istringstream str(Line);
				std::string word;
				str >> word;
				str >> word;
				str >> word;
				str >> word;
				str >> word;
				ParaType paratype = ShaderStruct::ParseType(word);
				str >> word;
				shader_struct_list[_type].SetAB(layout, paratype, word.erase(word.size() - 1, 1));
			}
			else if (Line.find("out ") != std::string::npos) {
				std::istringstream str(Line);
				std::string word;
				str >> word;
				str >> word;
				str >> word;
				str >> word;
				str >> word;
				ParaType paratype = ShaderStruct::ParseType(word);
				str >> word;
				const std::string name = word.erase(word.size() - 1, 1);
				shader_struct_list[_type].SetPass(layout, paratype, name);
			}
			else if (Line.find("buffer ") != std::string::npos) {
				layout = std::atoi(Line.substr(25, 1).c_str());
				std::istringstream str(Line);
				std::string name;
				LOOP(6)
					str >> name;

				while (Line != "};") {
					getline(_stream, Line);

					if (Line == "};") break;

					std::istringstream str(Line);
					std::string word;
					str >> word;

					ParaType paratype = ShaderStruct::ParseType(word);
					str >> word;
					word.erase(word.end() - 1);
					args_cache.emplace_back(paratype, word);
				};

				shader_struct_list[_type].SetSB(layout, name, args_cache);

				args_cache = {};
			}
			else if (Line.find("uniform ") != std::string::npos) {
				int start = Line.find("uniform ") + 8;
				std::string type_name = Line.substr(start, Line.find(" {") - start);
				while (Line.find("}") == std::string::npos) {
					getline(_stream, Line);

					if (Line.find("}") != std::string::npos) break;

					std::istringstream str(Line);
					std::string word;
					str >> word;

					ParaType paratype = ShaderStruct::ParseType(word);
					str >> word;
					word.erase(word.end() - 1);
					args_cache.emplace_back(paratype, word);
				}
				start = Line.find("} ") + 2;
				std::string var_name = Line.substr(start, Line.find(";") - start);

				shader_struct_list[_type].SetUB(type_name, var_name, args_cache);

				args_cache = {};
			}
		}
		else if (Line.find("in ") != std::string::npos) {
			// [in]
			// shader_struct_list[_type].SetInp(ShaderStruct::ParseType(Line.substr(3, 4)), 1, Line.substr(8, Line.size() - 9));
			// it is unnecessary to parese the input
		}
		else if (Line.find("out ") != std::string::npos) {
			// [out]
			std::istringstream str(Line);
			std::string name;			
			std::string type;
			str >> name;
			str >> type;
			str >> name;
			assert(!_is_link_repeat(name));
			ParaType para_type = ShaderStruct::ParseType(type);

			shader_struct_list[_type].SetOut(para_type, 1, name);
			shader_struct_list[1 - _type].SetInp(para_type, 1, name);

			_LINK_LOC[name] = _LINK_LOC.size();
		}
		else if (Line.find("uniform") != std::string::npos) {
			// [Uniform]
			std::istringstream str(Line);
			std::string word;
			str >> word;
			str >> word;
			ParaType paratype = ShaderStruct::ParseType(word);
			str >> word;
			word.erase(word.end() - 1);
			int count = 1;
			if (word.find("[") != std::string::npos) {
				count = std::atoi(word.substr(word.find("[") + 1, word.find("]") - word.find("[") - 1).c_str());
				word = word.substr(0, word.find("["));
				//DEBUG(word)
			}
			shader_struct_list[_type].SetUni(paratype, count, word);
		}
		else if (Line.find("struct") != std::string::npos) {
			// [Sturct]
			std::string name = Line.substr(7, Line.size() - 8);
			ShaderStruct::ADD_TYPE(name);
			while (Line != "};") {
				getline(_stream, Line);

				if (Line == "};") break;

				if (Line.find("//") != std::string::npos || Line == "")
					continue;

				std::istringstream str(Line);
				std::string word;
				str >> word;
				ParaType paratype = ShaderStruct::ParseType(word);
				str >> word;
				word.erase(word.end() - 1);
				args_cache.emplace_back(paratype, word);
			}

			shader_struct_list[_type].DefStruct(name, args_cache);

			args_cache = {};
		}
		else if (Line.find("const") != std::string::npos) {
			// [Const]
			std::istringstream str(Line);
			std::string word;
			str >> word;
			str >> word;
			ParaType paratype = ShaderStruct::ParseType(word);
			str >> word;
			std::string name = word;
			std::string content = Line.substr(Line.find(" = ") + 3, Line.size() - Line.find(" = ") - 4);
			shader_struct_list[_type].SetConst(paratype, name, content);
		}
		else if (Line.find("void main") != std::string::npos) {
			int blanc_count = (Line.find("{") != std::string::npos) ? 1 : 0;
			do {
				getline(_stream, Line);
				if (Line.find("{") != std::string::npos) blanc_count++;
				if (Line.find("}") != std::string::npos) blanc_count--;
				//shaders[type] << Line << "\n";
				shader_struct_list[_type].Main += Line + "\n";

			} while (blanc_count != 0);
			shader_struct_list[_type].Main.erase(shader_struct_list[_type].Main.size() - 3, 3);
		}
		else {
			std::istringstream str(Line);
			std::string word;
			str >> word;
			if (ShaderStruct::IsAvailType(word)) {
				if (Line.find("(") != std::string::npos) {
					// [Functions]
					int left_b = Line.find("(");
					ParaType paratype = ShaderStruct::ParseType(word);
					std::string name = Line.substr(word.size() + 1, left_b - word.size() - 1);
					std::string args_list = Line.substr(left_b + 1, Line.find(")") - left_b - 1) + ", ";
					//DEBUG(args_list)
					int blanc_count = Line.find("{") != std::string::npos ? 1 : 0;
					do {
						getline(_stream, Line);
						//shaders[type] << Line << "\n";
						if (blanc_count != 0)
							cache += Line + "\n";
						if (Line.find("{") != std::string::npos) blanc_count++;
						if (Line.find("}") != std::string::npos) blanc_count--;

					} while (blanc_count != 0);
					cache.erase(cache.size() - 1, 1);
					if (cache.substr(cache.size() - 2, 2).find("}") != std::string::npos)
						cache.erase(cache.size() - 2, 2);
					//shader_struct_list[_type].Main.erase(shader_struct_list[_type].Main.size() - 3, 3);
					shader_struct_list[_type].DefFunc(paratype, name, cache, ShaderStruct::ParseArgs(args_list));
					cache = "";
					args_cache = {};
				}
				else {
					// [Glob]
					int count = 1;
					std::string type_name = word;
					str >> word;
					if (word.find("[") != std::string::npos) {
						count = std::atoi(word.substr(word.find("[") + 1, word.find("]") - word.find("[") - 1).c_str());
						word = word.substr(0, word.find("["));
						//DEBUG(word)
					}
					else if (word.find(",") != std::string::npos) {
						do {
							shader_struct_list[_type].SetVar(type_name, word.erase(word.size() - 1, 1), count);
						} while (str >> word);
						shader_struct_list[_type].vari_list.pop_back();
					}
					else {
						word.erase(word.size() - 1, 1);
					}
					shader_struct_list[_type].SetVar(type_name, word, count);
				}
			}
		}
	}
}



std::string ShaderLib::GenerateShader(ShaderType tar /*= NONE_SHADER*/)
{
	int count = tar == NONE_SHADER ? 2 : 1;
	int offset = tar == NONE_SHADER ? 0 : (int)tar;
	ShaderType type = VERTEX_SHADER;
	LOOP(count) {
		type = (ShaderType)((int)type + offset + i);
		if (shader_struct_list[type].is_struct_changed) {
			shader_struct_list[type].is_struct_changed = false;
			shader_list[type] = ""; // RESET
			shader_list[type] += "#version " + std::to_string(shader_struct_list[type].version) + " core\n\n";

			std::string code_block = "";
			if (shader_struct_list[type].AB_list.size()) {
				code_block += "// [ARRAY_BUFFER]\n";
				for (const auto& [loc, name, type] : shader_struct_list[type].AB_list)
					code_block += "layout(location = " + std::to_string(loc) + ") in " + ShaderStruct::ParseType(type)/* + " In_"*/ + " " + name + ";\n";
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].pass_list.size()) {
				code_block += "// [RENDER_BUFFER]\n";
				for (const auto& [loc, name, type] : shader_struct_list[type].pass_list)
					code_block += "layout(location = " + std::to_string(loc) + ") out " + ShaderStruct::ParseType(type)/* + " Ch_"*/ + " " + name + ";\n";
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].struct_def_list.size()) {
				code_block += "// [STRUCTURE_DEFINE]\n";
				for (const auto& [_, sname, args] : shader_struct_list[type].struct_def_list) {
					code_block += "struct " + sname + "{\n";
					for (const auto& [type, name] : args) {
						code_block += "\t" + ShaderStruct::ParseType(type) + " " + name + ";\n";
					}
					code_block += "};\n\n";
				}
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].SB_list.size()) {
				code_block += "// [STORAGE_BUFFER]\n";
				for (const auto& [loc, bname, args] : shader_struct_list[type].SB_list) {
					code_block += "layout(std430, binding = " + std::to_string(loc) + ") buffer " +bname + " {\n";
					for (const auto& [type, name] : args) {
						code_block += "\treadonly " + ShaderStruct::ParseType(type) + " " + name + ";\n";
					}
					code_block += "};\n";
				}
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].ubuffer_list.size()) {
				code_block += "// [UNIFORM BUFFER]\n";
				for (const auto& [_, uname, vname, args] : shader_struct_list[type].ubuffer_list) {
					code_block += "layout(std140) uniform " + uname + " {\n";
					for (const auto& [type, name] : args) {
						code_block += "\t" + ShaderStruct::ParseType(type) + " " + name + ";\n";
					}
					code_block += "} " + vname + ";\n";
				}
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].uniform_list.size()) {
				code_block += "// [UNIFORM]\n";
				for (const auto& [name, type, count] : shader_struct_list[type].uniform_list)
					code_block += "uniform " + ShaderStruct::ParseType(type) /*+ " U_" */ + " " + name + ShaderStruct::ParseCount(count) + ";\n";
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].input_list.size()) {
				code_block += "// [IN]\n";
				for (const auto& [name, type, count] : shader_struct_list[type].input_list)
					code_block += "in " + ShaderStruct::ParseType(type) /*+ " Out_" */ + " " + name + ShaderStruct::ParseCount(count) + ";\n";
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].output_list.size()) {
				code_block += "// [OUT]\n";
				for (const auto& [name, type, count] : shader_struct_list[type].output_list)
					code_block += "out " + ShaderStruct::ParseType(type) /*+ " Out_" */ + " " + name + ShaderStruct::ParseCount(count) + ";\n";
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].glob_list.size()) {
				code_block += "// [GLOBES]\n";
				for (const auto& [name, type, val] : shader_struct_list[type].glob_list)
					code_block += ShaderStruct::ParseType(type) /* + " Glob_"*/ + name + " = " + ShaderStruct::ParseType(type) + "(" + std::to_string(val) + ");\n";
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].const_list.size()) {
				code_block += "// [CONSTS]\n";
				for (const auto& [type, name, cont, _] : shader_struct_list[type].const_list)
					code_block += "const " + ShaderStruct::ParseType(type) + " " + name + " = " + cont + ";\n";
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].vari_list.size()) {
				code_block += "// [VARS]\n";
				for (const auto& [tname, name, count] : shader_struct_list[type].vari_list)
					code_block += tname + " " + name + ShaderStruct::ParseCount(count) + ";\n";
				code_block += "\n";
			}
			shader_list[type] += code_block;

			code_block = "";
			if (shader_struct_list[type].func_list.size()) {
				code_block += "// [FUNCTION_DEFINE]\n";
				for (const auto& [rtype, name, cont, args] : shader_struct_list[type].func_list) {
					code_block += ShaderStruct::ParseType(rtype) + " " + name + ShaderStruct::ParseArgs(args) + "{\n";
					code_block += cont;
					code_block += "\n}\n\n";
				}
			}
			shader_list[type] += code_block;

			shader_list[type] += "\nvoid main(){\n";
			shader_list[type] += shader_struct_list[type].Main;
			shader_list[type] += "\n};\n";
		}
		shader_struct_list[type].is_struct_changed = true;
	}


	return "Genertated";
}