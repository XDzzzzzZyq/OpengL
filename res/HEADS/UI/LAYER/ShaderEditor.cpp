#include "ShaderEditor.h"

std::string const ShaderEditor::edit_mode[3] = { "Shader Code", "Hierarchy", "Nodes" };
std::string const ShaderEditor::shader_type[2] = { "Vertex Shader", "Fragment Shader" };

TextEditor ShaderEditor::Editor = TextEditor();

ShaderEditor::ShaderEditor()
	:ShaderEditor("Shader Editor")
{

}

ShaderEditor::ShaderEditor(const std::string& name)
{
	uly_name = name;
	//Editor.SetLanguageDefinition(TextEditor::LanguageDefinition().GLSL());
}

ShaderEditor::~ShaderEditor()
{

}

bool ShaderEditor::AddParam(const char* c_name, const char* c_sld_name) const
{

	add_button = ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)) || add_button;

	if (add_button)
		if (Panel.RenderPanel(ImVec2(mouse_x, mouse_y), &add_button, &add_prop, c_name, c_sld_name)) {
			return true;
		}

	return false;
}

bool ShaderEditor::AddStruct(bool def_type) const
{
	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)) || add_button)
	{
		add_button = true;

		if (Panel.RenderDefPanel(def_type, ImVec2(mouse_x, mouse_y), &add_button, &add_args)) {
			return true;
		}

	}
	return false;
}

void ShaderEditor::UpdateLayer()
{
	if (is_selected_changed || is_shad_type_changed) {
		UpdateShaderEditor();
		is_shad_type_changed = false;
	}

}

void ShaderEditor::RenderName(const std::string& _label, std::string & _name, bool read_only) const
{
	char name[CHAR_MAX];
	_name.copy(name, CHAR_MAX, 0);
	*(name + _name.size()) = '\n';
	ImGui::InputText(_label.c_str(), name, CHAR_MAX, read_only?ImGuiInputTextFlags_ReadOnly:0);

	if (!read_only)
		_name = std::string(name);
}

void ShaderEditor::RenderName(const char* _label, std::string& _name, bool read_only /*= true*/) const
{
	char name[CHAR_MAX];
	_name.copy(name, CHAR_MAX, 0);
	*(name + _name.size()) = '\n';
	ImGui::InputText(_label, name, CHAR_MAX, read_only ? ImGuiInputTextFlags_ReadOnly : 0);

	if (!read_only)
		_name = std::string(name);
}

void ShaderEditor::RenderShaderStruct() const
{
	int type_id = 0, vari_id = 0;
	// [BASE INFO]
	ImGui::PushID(type_id++);

	if (ImGui::TreeNode("Base Information")) {
		ImGui::Text("========================");
		ImGui::Text("GLSL version : %i", active_shader->shader_struct_list[current_shad_type].version);
		ImGui::Text("Shader Type : " + current_shad_type == 0 ? "Vertex Shader" : "Fragment Shader");
		ImGui::Text(("Shader ID : " + std::to_string(active_shader->getShaderID((ShaderType)current_shad_type))).c_str());
		ImGui::Text(active_shader->shader_struct_list[current_shad_type].is_struct_changed ? "Status : Changed" : "Status : Compiled");
		ImGui::Text("========================");
		ImGui::TreePop();
	}
	ImGui::PopID();
	// [AB]
	if (active_shader->shader_struct_list[current_shad_type].AB_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Array Buffer")) {
			for (auto& l_in : active_shader->shader_struct_list[current_shad_type].AB_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(l_in).c_str())) {
					RenderLayout(&std::get<0>(l_in), &std::get<1>(l_in), &std::get<2>(l_in));
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddParam("Array Buffer", "layout")) {
				if (active_shader)
					active_shader->shader_struct_list[current_shad_type].SetAB(std::get<2>(add_prop), std::get<1>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
		}ImGui::PopID();vari_id = 0;
	}

	// [PASS]
	if (active_shader->shader_struct_list[current_shad_type].pass_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Rendering Pass")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].pass_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddParam("Render Pass", "layout")) {
				if (active_shader)
					active_shader->shader_struct_list[current_shad_type].SetPass(std::get<2>(add_prop), std::get<1>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
		}ImGui::PopID();vari_id = 0;
	}

	//[IN]
	if (active_shader->shader_struct_list[current_shad_type].input_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Input")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].input_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddParam("Input")) {
				if (active_shader)
					active_shader->shader_struct_list[current_shad_type].SetOut(std::get<1>(add_prop), std::get<2>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
		}ImGui::PopID();vari_id = 0;
	}

	//[OUT]
	if (active_shader->shader_struct_list[current_shad_type].output_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Output")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].output_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddParam("Output")) {
				if (active_shader)
					active_shader->shader_struct_list[current_shad_type].SetOut(std::get<1>(add_prop), std::get<2>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
		}ImGui::PopID();vari_id = 0;
	}

	//[UNIFORM]
	if (active_shader->shader_struct_list[current_shad_type].uniform_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Uniform")) {
			int index = 0;
			for (auto& i : active_shader->shader_struct_list[current_shad_type].uniform_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					RenderArg(std::get<1>(i), std::get<0>(i), index++, false);
					ImGui::TreePop();
				}ImGui::PopID();
			}
			//ImGui::PushFont(ImguiTheme::th_data.font_data[0]);
			if (AddParam("Uniforms")) {
				if (active_shader)
					active_shader->shader_struct_list[current_shad_type].SetUni(std::get<1>(add_prop), std::get<2>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
			//ImGui::PopFont();
		}ImGui::PopID();vari_id = 0;
	}

	//[STRUCT]
	if (active_shader->shader_struct_list[current_shad_type].struct_def_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Struct")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].struct_def_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					RenderName("struct name", std::get<1>(i));
					RenderArgs(std::get<2>(i), 0);
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddStruct()) {
				if (active_shader)
					active_shader->shader_struct_list[current_shad_type].DefStruct(std::get<1>(add_args), std::get<3>(add_args));
			}ImGui::TreePop();
		}ImGui::PopID();vari_id = 0;
	}

	//[Globs]
	if (active_shader->shader_struct_list[current_shad_type].glob_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Globs")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].glob_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{

			}ImGui::TreePop();
		}ImGui::PopID();vari_id = 0;
	}

	//[Const]
	if (active_shader->shader_struct_list[current_shad_type].const_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Consts")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].const_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{

			}ImGui::TreePop();
		}ImGui::PopID();vari_id = 0;
	}

	//[Funcs]
	if (active_shader->shader_struct_list[current_shad_type].func_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Functions")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].func_list) {
				bool op_ev = false, st_ev = false;
				ImGui::PushID(vari_id++);
				bool is_op = ImGui::TreeNodeB(
					std::get<1>(i).c_str(),
					&active_shader->shader_struct_list[current_shad_type].func_list_state[vari_id - 1],
					&op_ev,
					&st_ev,
					0,
					(active_func!=vari_id)
				);
				if (op_ev) {
					Editor.SetText(std::get<2>(i));
					active_func = vari_id;
				}
				if (active_func==vari_id && is_op) {
					RenderName("func name", std::get<1>(i));

					if (ImGui::TreeNode("arguments list")) {
						RenderArgs(std::get<3>(i), -1);
						ImGui::TreePop();
					}
					if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x, 20))) {
						std::get<2>(i) = Editor.GetText();
					}
					Editor.Render(std::get<1>(i).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 100), true);
				}ImGui::PopID();
				if(is_op)ImGui::TreePop();
				if (st_ev) {
					if(Editor.IsTextChanged())
						std::get<2>(i) = Editor.GetText();
					active_shader->shader_struct_list[current_shad_type].is_struct_changed = true;
				}
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{

			}ImGui::TreePop();
		}ImGui::PopID();vari_id = 0;
	}

	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 5, 20)))
	{

	}
}

void ShaderEditor::RenderLayout(int* _loc, std::string* _name, ParaType* _type)const
{
	ImGui::SetNextItemWidth(80);
	ImGui::InputInt(_name->c_str(), _loc, 1, 100, ImGuiInputTextFlags_NoName);ImGui::SameLine();ImGui::SetNextItemWidth(60);
	if (ImGui::BeginCombo(std::to_string(*_type).c_str(), ShaderStruct::ParseType((ParaType)*_type).c_str(), ImGuiComboFlags_NoName)) {
		LOOP(ShaderStruct::type_table.size())
			if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), &sel))
				*_type = (ParaType)i;
		ImGui::EndCombo();
	}ImGui::SameLine();
	_name->copy(add_name, CHAR_MAX, 0);
	*(add_name + _name->size()) = '\0';ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputText((*_name + "name").c_str(), add_name, CHAR_MAX, ImGuiInputTextFlags_NoName | ImGuiInputTextFlags_ReadOnly);
	*_name = std::string(add_name);
}

void ShaderEditor::RenderSSBO(int* _loc, std::string* _name, ParaType* _type, Args* _args) const
{
	RenderLayout(_loc, _name, _type);
	
}

void ShaderEditor::RenderArg(Arg& _arg, int _index, bool _is_editable) const
{
	char name[CHAR_MAX];
	std::string b_name = _arg.second;
	b_name.copy(name, b_name.size());
	*(name + b_name.size()) = '\0';
	ImGui::SetNextItemWidth(100);
	if (ImGui::BeginCombo(std::to_string(_index).c_str(), ShaderStruct::ParseType(_arg.first).c_str(), ImGuiComboFlags_NoName)) {
		LOOP(ShaderStruct::type_table.size())
			if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), &sel))
				_arg.first = (ParaType)i;
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::InputTextMultiline(("m" + std::to_string(_index)).c_str(), name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20), ImGuiInputTextFlags_NoName | (!_is_editable * ImGuiInputTextFlags_ReadOnly));

	_arg.second = std::string(name);
}

void ShaderEditor::RenderArg(ParaType& _type, std::string& _name, int _index, bool _is_editable) const
{
	char name[CHAR_MAX];
	_name.copy(name, _name.size());
	*(name + _name.size()) = '\0';
	ImGui::SetNextItemWidth(100);
	if (ImGui::BeginCombo(std::to_string(_index).c_str(), ShaderStruct::ParseType(_type).c_str(), ImGuiComboFlags_NoName)) {
		LOOP(ShaderStruct::type_table.size())
			if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), &sel))
				_type = (ParaType)i;
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::InputTextMultiline(("m" + std::to_string(_index)).c_str(), name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20), ImGuiInputTextFlags_NoName | (!_is_editable * ImGuiInputTextFlags_ReadOnly));

	_name = std::string(name);
}

void ShaderEditor::RenderArgs(Args& args, int _type) const
{
	int index = 0;
	for (auto& arg : args)
		RenderArg(arg, index++, _type!=-1);
}

void ShaderEditor::UpdateShaderEditor(const std::string& _code) const {
	if (active_shader)
		if(current_edit == CODE_EDITOR)
			Editor.SetText(active_shader->shader_list[current_shad_type]);
		else if(current_edit == STRUCT_EDITOR)
			Editor.SetText(_code);
}

void ShaderEditor::CompileShader() const {
	if (active_shader) {
		
		Timer timer;
		switch (current_edit) {
		case CODE_EDITOR:
			if (Editor.GetText() == active_shader->shader_list[(ShaderType)current_shad_type]) return;
			active_shader->shader_list[(ShaderType)current_shad_type] = Editor.GetText();
			dynamic_cast<Shaders*>(active_shader)->ParseShaderCode("", (ShaderType)current_shad_type);
			break;
		case STRUCT_EDITOR:
			is_shad_type_changed = true;
			active_shader->GenerateShader((ShaderType)current_shad_type);break;
		}

		glDeleteProgram(active_shader->getID());
		glDeleteShader(dynamic_cast<Shaders*>(active_shader)->getShaderID((ShaderType)current_shad_type));

		GLuint program_id = glCreateProgram();

		GLuint shader_id = active_shader->CompileShader((ShaderType)current_shad_type);
		glAttachShader(program_id, shader_id);
		glAttachShader(program_id, dynamic_cast<Shaders*>(active_shader)->getShaderID(current_shad_type == VERTEX_SHADER ? FRAGMENT_SHADER : VERTEX_SHADER));

		glLinkProgram(program_id);
		glValidateProgram(program_id);

		int link_state = -1;
		glGetProgramiv(program_id, GL_LINK_STATUS, &link_state);

		if (link_state != GL_TRUE)
			DEBUG("Shader Link Error")

		dynamic_cast<Shaders*>(active_shader)->ResetID((ShaderType)current_shad_type, shader_id);
		dynamic_cast<Shaders*>(active_shader)->ResetID((ShaderType)NONE_SHADER, program_id);
		dynamic_cast<Shaders*>(active_shader)->ResetCache();

		active_shader->is_shader_changed = true;
	}

}

void ShaderEditor::RenderLayer() const
{
	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {
		if (ImGui::BeginCombo("Edit Mode", edit_mode[current_edit].c_str())) {
			LOOP(3)
				if (ImGui::Selectable(edit_mode[i].c_str(), &sel)) {
					current_edit = i;
					if (active_shader)active_shader->shader_struct_list[current_shad_type].is_struct_changed = true;
				}

			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Shader Type", shader_type[current_shad_type].c_str())) {
			LOOP(2)
				if (ImGui::Selectable(shader_type[i].c_str(), &sel)) {
					current_shad_type = i;
					is_shad_type_changed = true;
				}

			ImGui::EndCombo();
		}
		if (ImGui::Button("Compile", ImVec2(ImGui::GetContentRegionAvail().x / 2, 25))) {
			CompileShader();
		}ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 25))) {
			//active_shader->GenerateShader((ShaderType)current_shad_type);
			//DEBUG(active_shader->getID())
		}
		//editor.SetLanguageDefinition(TextEditor::LanguageDefinition().GLSL());

		switch (current_edit)
		{
		case CODE_EDITOR:

			if (active_shader) {
				Editor.Render("##Editor", ImGui::GetContentRegionAvail());
			}

			break;
		case STRUCT_EDITOR:
			if (active_shader)
				RenderShaderStruct();
			break;
		case 2:
			break;
		}
		ImGui::End();
	}
	else {
		ImGui::End();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MiniPropPanel::RenderPanel(const ImVec2& pos, bool* state, S_U* out, const char* c_name, const char* c_sld_name)
{
	if (!is_open)
		panel_pos = pos;
	is_open = true;
	ImGui::SetNextWindowPos(ImGui::GetWindowPos() + panel_pos - ImVec2(100, 0));
	ImGui::SetNextWindowSize(ImVec2(300, 80));
	bool a = false, b = false;

	if (ImGui::Begin("ADD", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {
		a = ImGui::IsWindowFocused();


		ImGui::SetNextItemWidth(100);
		if (ImGui::BeginCombo(" ", ShaderStruct::type_table[datatype].c_str())) {
			b = true;
			LOOP(ShaderStruct::type_table.size())
				if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), &a))
					datatype = i;
			ImGui::EndCombo();
		}ImGui::SameLine();
		if ((!a) && (!b))
		{
			ImGui::End();
			*state = is_open = false;
			return false;
		}

		ImGui::InputTextMultiline(c_name, &prop_name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20));

		ImGui::InputInt(c_sld_name == "" ? "count" : c_sld_name, &prop_count);

		if (ImGui::Button("OK", ImGui::GetContentRegionAvail())) {
			is_open = *state = false;
			ImGui::End();
			if (datatype == 0 || prop_name == NULL)
				return false;

			*out = { std::string(&prop_name), ParaType(datatype), prop_count };
			prop_name = NULL;
			return true;
		}
		ImGui::End();
	}
	return false;
}

bool MiniPropPanel::RenderDefPanel(bool type, const ImVec2& pos, bool* state, S_func* _struct)
{
	if (*state) {
		if (!is_open)
			panel_pos = pos;
		is_open = true;
		ImGui::SetNextWindowPos(ImGui::GetWindowPos() + panel_pos - ImVec2(100, 0));
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		bool a, b = false;

		if (ImGui::Begin("Add Def", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav)) {
			if (type) {
				ImGui::SetNextItemWidth(100);
				if (ImGui::BeginCombo(" ", ShaderStruct::type_table[datatype].c_str())) {
					LOOP(ShaderStruct::type_table.size())
						if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), &a))
							datatype = i;
					ImGui::EndCombo();
				}ImGui::SameLine();
			}

			ImGui::InputTextMultiline("name", &prop_name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20));

			RenderArguPanel(&b);

			if ((!ImGui::IsWindowFocused()) && (!b)) {
				*state = is_open = false;
				ImGui::End();
				return false;
			}

			if (type) {
				ImGui::InputTextMultiline("content", &prop_content, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 40));
			}

			if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
				is_open = *state = false;
				ImGui::End();
				if (datatype == 0 || prop_name == NULL)
					return false;

				return true;
			}
			ImGui::End();
		}
	}
	return false;
}

void MiniPropPanel::RenderArguPanel(bool* b)
{
	int index = 0;
	for (auto& arg : prop_args) {
		char name[CHAR_MAX];
		std::string b_name = std::get<1>(arg);
		b_name.copy(name, b_name.size());
		*(name + b_name.size()) = '\0';
		int type = std::get<0>(arg);
		bool a;
		ImGui::SetNextItemWidth(100);
		if (ImGui::BeginCombo(("member" + std::to_string(index)).c_str(), ShaderStruct::type_table[type].c_str())) {
			*b = true;
			LOOP(ShaderStruct::type_table.size())
				if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), &a))
					type = i;
			ImGui::EndCombo();
		}ImGui::SameLine();
		ImGui::InputTextMultiline(("m_name" + std::to_string(index)).c_str(), name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20));

		arg = { (ParaType)type, std::string(name) };
		index++;
	}
	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x, 40)))
		prop_args.emplace_back(NONE_PARA, "Empty");
}

char MiniPropPanel::prop_name = NULL;
