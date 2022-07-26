#include "Shaders.h"
#include "ShaderLib.h"

void Shaders::ParseShaderFile(const std::string& _name, ShaderType _type) {
	Timer timer("ParseShader");

	active_shader = _type;
	shader_struct_list[active_shader].is_struct_changed = false;
	std::ifstream Stream(ShaderLib::folder_root + _name + ShaderLib::file_type[_type]);

	ParseShaderStream(Stream, active_shader);

	shader_struct_list[active_shader].func_list_state.resize(shader_struct_list[active_shader].func_list.size());


	std::cout << "shaders are loaded up successfully!" << std::endl;
	//m_shaders = { shaders[0].str(),shaders[1].str() };
}


void Shaders::ParseShaderStream(std::istream& _stream, ShaderType _type)
{
	std::string Line;
	std::string cache = "";
	Args args_cache;

	int line_count = 0;
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
			int layout = std::atoi(Line.substr(18, 1).c_str());
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
				shader_struct_list[_type].SetPass(layout, paratype, word.erase(word.size() - 1, 1));
			}
			else if (Line.find("buffer ") != std::string::npos) {
				layout = std::atoi(Line.substr(25, 1).c_str());
				std::istringstream str(Line);
				std::string name;
				LOOP(6)
					str >> name;

				do {
					getline(_stream, Line);
					//shaders[type] << Line << "\n";
					cache += Line + "\n";

					std::istringstream str(Line);
					std::string word;
					str >> word;

					if (word != "};") {
						ParaType paratype = ShaderStruct::ParseType(word);
						str >> word;
						word.erase(word.end() - 1);
						args_cache.emplace_back(paratype, word);
					}
				} while (Line != "};");

				shader_struct_list[_type].SetSB(layout, name, args_cache);

				args_cache = {};
				cache = "";
			}
		}
		else if (Line.find("in ") != std::string::npos) {
			// [in]
			// shader_struct_list[_type].SetInp(ShaderStruct::ParseType(Line.substr(3, 4)), 1, Line.substr(8, Line.size() - 9));
			// it is unnecessary to parese the input
		}
		else if (Line.find("out ") != std::string::npos) {
			// [out]
			std::string name = Line.substr(9, Line.size() - 10);
			assert(!_is_link_repeat(name));
			ParaType type = ShaderStruct::ParseType(Line.substr(4, 4));
			shader_struct_list[_type].SetOut(type, 1, name);
			shader_struct_list[1 - _type].SetInp(type, 1, name);

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
			std::string name = Line.substr(7, Line.size() - 9);
			ShaderStruct::ADD_TYPE(name);
			do {
				getline(_stream, Line);
				//shaders[type] << Line << "\n";
				cache += Line + "\n";

				std::istringstream str(Line);
				std::string word;
				str >> word;

				if (word != "};") {
					ParaType paratype = ShaderStruct::ParseType(word);
					str >> word;
					word.erase(word.end() - 1);
					args_cache.emplace_back(paratype, word);
				}
			} while (Line != "};");

			shader_struct_list[_type].DefStruct(name, args_cache);

			args_cache = {};
			cache = "";
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
			int blanc_count = 1;
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



			if (shader_struct_list[type].AB_list.size()) {
				shader_list[type] += "// [ARRAY_BUFFER]\n";
				for (const auto& i : shader_struct_list[type].AB_list)
					shader_list[type] += "layout(location = " + std::to_string(std::get<0>(i)) + ") in " + ShaderStruct::ParseType(std::get<2>(i))/* + " In_"*/ + " " + std::get<1>(i) + ";\n";
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].SB_list.size()) {
				shader_list[type] += "// [STORAGE_BUFFER]\n";
				for (const auto& i : shader_struct_list[type].SB_list) {
					shader_list[type] += "layout(std430, binding = " + std::to_string(std::get<0>(i)) + ") buffer " + std::get<1>(i) + " {\n";
					for (const auto& j : std::get<2>(i)) {
						shader_list[type] += "\t" + ShaderStruct::ParseType(j.first) + " " + j.second + ";\n";
					}
					shader_list[type] += "};\n";
				}
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].pass_list.size()) {
				shader_list[type] += "// [RENDER_BUFFER]\n";
				for (const auto& i : shader_struct_list[type].pass_list)
					shader_list[type] += "layout(location = " + std::to_string(std::get<0>(i)) + ") out " + ShaderStruct::ParseType(std::get<2>(i))/* + " Ch_"*/ + " " + std::get<1>(i) + ";\n";
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].uniform_list.size()) {
				shader_list[type] += "// [UNIFORM]\n";
				for (const auto& i : shader_struct_list[type].uniform_list)
					shader_list[type] += "uniform " + ShaderStruct::ParseType(std::get<1>(i)) /*+ " U_" */ + " " + std::get<0>(i) + ShaderStruct::ParseCount(std::get<2>(i)) + ";\n";
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].input_list.size()) {
				shader_list[type] += "// [IN]\n";
				for (const auto& i : shader_struct_list[type].input_list)
					shader_list[type] += "in " + ShaderStruct::ParseType(std::get<1>(i)) /*+ " Out_" */ + " " + std::get<0>(i) + ShaderStruct::ParseCount(std::get<2>(i)) + ";\n";
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].output_list.size()) {
				shader_list[type] += "// [OUT]\n";
				for (const auto& i : shader_struct_list[type].output_list)
					shader_list[type] += "out " + ShaderStruct::ParseType(std::get<1>(i)) /*+ " Out_" */ + " " + std::get<0>(i) + ShaderStruct::ParseCount(std::get<2>(i)) + ";\n";
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].struct_def_list.size()) {
				shader_list[type] += "// [STRUCTURE_DEFINE]\n";
				for (const auto& i : shader_struct_list[type].struct_def_list) {
					shader_list[type] += "struct " + std::get<1>(i) + "{\n";
					for (const auto& j : std::get<2>(i)) {
						shader_list[type] += "\t" + ShaderStruct::ParseType(j.first) + " " + j.second + ";\n";
					}
					shader_list[type] += "};\n\n";
				}
			}

			if (shader_struct_list[type].glob_list.size()) {
				shader_list[type] += "// [GLOBES]\n";
				for (const auto& i : shader_struct_list[type].glob_list)
					shader_list[type] += ShaderStruct::ParseType(std::get<1>(i)) /* + " Glob_"*/ + std::get<0>(i) + " = " + ShaderStruct::ParseType(std::get<1>(i)) + "(" + std::to_string(std::get<2>(i)) + ");\n";
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].const_list.size()) {
				shader_list[type] += "// [CONSTS]\n";
				for (const auto& i : shader_struct_list[type].const_list)
					shader_list[type] += "const " + ShaderStruct::ParseType(std::get<0>(i)) + " " + std::get<1>(i) + " = " + std::get<2>(i) + ";\n";
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].vari_list.size()) {
				shader_list[type] += "// [VARS]\n";
				for (const auto& i : shader_struct_list[type].vari_list)
					shader_list[type] += std::get<0>(i) + " " + std::get<1>(i) + ShaderStruct::ParseCount(std::get<2>(i)) + ";\n";
				shader_list[type] += "\n";
			}

			if (shader_struct_list[type].func_list.size()) {
				shader_list[type] += "// [FUNCTION_DEFINE]\n";
				for (const auto& i : shader_struct_list[type].func_list) {
					shader_list[type] += ShaderStruct::ParseType(std::get<0>(i)) + " " + std::get<1>(i) + ShaderStruct::ParseArgs(std::get<3>(i)) + "{\n";
					shader_list[type] += std::get<2>(i);
					shader_list[type] += "\n}\n\n";
				}
			}

			shader_list[type] += "\nvoid main(){\n";
			shader_list[type] += shader_struct_list[type].Main;
			shader_list[type] += "};\n";
		}
		shader_struct_list[type].is_struct_changed = true;
	}


	return "Genertated";
}