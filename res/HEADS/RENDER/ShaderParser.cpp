#include "Shaders.h"
#include "ShaderLib.h"

void Shaders::ParseShader(const std::string& name, const std::string& name2) {
	Timer timer("ParseShader");
	//std::stringstream shaders[2];

	ShaderType type = VERTEX_SHADER;
	LOOP(2) {

		active_shader = type;
		shader_struct_list[active_shader].is_struct_changed = false;
		std::ifstream Stream(ShaderLib::folder_root + name + ShaderLib::file_type[type]);
		std::string Line;
		std::string cache = "";
		Args args_cache;

		int line_count = 0;
		while (getline(Stream, Line)) {


			// PARSING
			//shaders[type] << Line << "\n";
			if (Line == "") {
			}
			else if (Line.find("#version") != std::string::npos) {
				// [Version]
				shader_struct_list[active_shader].version = std::atoi(Line.substr(9, 3).c_str());
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
					shader_struct_list[active_shader].SetAB(layout, paratype, word.erase(word.size() - 1, 1));
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
					shader_struct_list[active_shader].SetPass(layout, paratype, word.erase(word.size() - 1, 1));
				}
				else if (Line.find("buffer ") != std::string::npos) {
					layout = std::atoi(Line.substr(25, 1).c_str());
					std::istringstream str(Line);
					std::string name;
					LOOP(6)
						str >> name;

					do {
						getline(Stream, Line);
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

					shader_struct_list[active_shader].SetSB(layout, name, args_cache);

					args_cache = {};
					cache = "";
				}
			}
			else if (Line.find("in ") != std::string::npos) {
				// [in]
				shader_struct_list[active_shader].SetInp(ShaderStruct::ParseType(Line.substr(3, 4)), 1, Line.substr(8, Line.size() - 9));
			}
			else if (Line.find("out ") != std::string::npos) {
				// [out]
				shader_struct_list[active_shader].SetOut(ShaderStruct::ParseType(Line.substr(4, 4)), 1, Line.substr(9, Line.size() - 10));
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
				shader_struct_list[active_shader].SetUni(paratype, count, word);
			}
			else if (Line.find("struct") != std::string::npos) {
				// [Sturct]
				std::string name = Line.substr(7, Line.size() - 9);
				ShaderStruct::ADD_TYPE(name);
				do {
					getline(Stream, Line);
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

				shader_struct_list[active_shader].DefStruct(name, args_cache);

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
				shader_struct_list[active_shader].SetConst(paratype, name, content);
			}
			else if (Line.find("void main") != std::string::npos) {
				int blanc_count = 1;
				do {
					getline(Stream, Line);
					if (Line.find("{") != std::string::npos) blanc_count++;
					if (Line.find("}") != std::string::npos) blanc_count--;
					//shaders[type] << Line << "\n";
					shader_struct_list[active_shader].Main += Line + "\n";

				} while (blanc_count != 0);
				shader_struct_list[active_shader].Main.erase(shader_struct_list[active_shader].Main.size() - 3, 3);
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
						int blanc_count = Line.find("{") != std::string::npos?1:0;
						do {
							getline(Stream, Line);
							//shaders[type] << Line << "\n";
							if(blanc_count!=0)
								cache += Line + "\n";
							if (Line.find("{") != std::string::npos) blanc_count++;
							if (Line.find("}") != std::string::npos) blanc_count--;
						
						} while (blanc_count != 0);
						cache.erase(cache.size() - 1, 1);
						if (cache.substr(cache.size() - 2, 2).find("}")!=std::string::npos)
							cache.erase(cache.size() - 2, 2);
						//shader_struct_list[active_shader].Main.erase(shader_struct_list[active_shader].Main.size() - 3, 3);
						shader_struct_list[active_shader].DefFunc(paratype, name, cache, ShaderStruct::ParseArgs(args_list));
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
							do{
								shader_struct_list[active_shader].SetVar(type_name, word.erase(word.size() - 1, 1), count);
							} while (str >> word);
							shader_struct_list[active_shader].vari_list.pop_back();
						}
						else {
							word.erase(word.size() - 1, 1);
						}
						shader_struct_list[active_shader].SetVar(type_name, word, count);
					}
				}
			}
		}

		type = (ShaderType)((int)type + 1);
	}

	std::cout << "shaders are loaded up successfully!" << std::endl;
	//m_shaders = { shaders[0].str(),shaders[1].str() };
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
	}


	return "Genertated";
}