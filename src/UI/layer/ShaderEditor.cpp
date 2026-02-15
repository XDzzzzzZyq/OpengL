#include "ShaderEditor.h"
#include "Shaders.h"
#include "Input.h"

#include "operator.h"

#include "events/KeyMouseEvents.h"
#include "events/EditorEvents.h"

std::string const ShaderEditor::edit_mode[3] = { "Shader Code", "Hierarchy", "Nodes" };

TextEditor ShaderEditor::se_code_editor{};
NodeEditor ShaderEditor::se_node_editor{};

ShaderEditor::ShaderEditor()
	:ShaderEditor("Shader Editor")
{

}

ShaderEditor::ShaderEditor(const std::string& name)
{
	uly_name = name;
	//Editor.SetLanguageDefinition(TextEditor::LanguageDefinition().GLSL());
}

void ShaderEditor::InitEditors()
{
	ShaderEditor::se_code_editor.Init();
	ShaderEditor::se_node_editor = NodeEditor(SHADER_NODE_EDITOR);
}

ShaderEditor::~ShaderEditor()
{

}

static Args prop_args;
static S_U add_prop;
static S_func add_args;
static char prop_name = NULL;
static char prop_content = NULL;


/* Rendering Utils */

bool RenderPanel(ShaderEditor::MiniPropPanel& panel, const ImVec2& pos, S_U* out, const char* c_name, const char* c_sld_name)
{
	if (!panel.is_open) {
		panel.panel_pos = pos;
		panel.is_open = true;
	}
	ImGui::SetNextWindowPos(ImGui::GetWindowPos() + panel.panel_pos - ImVec2(100, 0));
	ImGui::SetNextWindowSize(ImVec2(300, 80));
	bool a = false, b = false;

	if (ImGui::Begin("ADD", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {
		a = ImGui::IsWindowFocused();


		ImGui::SetNextItemWidth(100);
		if (ImGui::BeginCombo(" ", ShaderStruct::type_table[panel.datatype].c_str())) {
			b = true;
			LOOP(ShaderStruct::type_table.size())
				if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), &a))
					panel.datatype = i;
			ImGui::EndCombo();
		}ImGui::SameLine();
		if ((!a) && (!b))
		{
			ImGui::End();
			panel.is_open = false;
			return false;
		}

		ImGui::InputTextMultiline(c_name, &prop_name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20));

		ImGui::InputInt(c_sld_name, &panel.prop_count);

		if (ImGui::Button("OK", ImGui::GetContentRegionAvail())) {
			panel.is_open = false;
			ImGui::End();
			if (prop_name == NULL)
				return false;

			*out = { std::string(&prop_name), ParaType(panel.datatype), panel.prop_count };
			prop_name = NULL;
			return true;
		}
		ImGui::End();
	}
	return false;
}

void RenderArguPanel(bool* b)
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

bool RenderDefPanel(ShaderEditor::MiniPropPanel& panel, bool type, const ImVec2& pos, S_func* _struct)
{
	if (!panel.is_open) {
		panel.panel_pos = pos;
		panel.is_open = true;
	}
	ImGui::SetNextWindowPos(ImGui::GetWindowPos() + panel.panel_pos - ImVec2(100, 0));
	ImGui::SetNextWindowSize(ImVec2(400, 300));
	bool a, b = false;

	if (ImGui::Begin("Add Def", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav)) {
		if (type) {
			ImGui::SetNextItemWidth(100);
			if (ImGui::BeginCombo(" ", ShaderStruct::type_table[panel.datatype].c_str())) {
				LOOP(ShaderStruct::type_table.size())
					if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), &a))
						panel.datatype = i;
				ImGui::EndCombo();
			}ImGui::SameLine();
		}

		ImGui::InputTextMultiline("name", &prop_name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20));

		RenderArguPanel(&b);

		if ((!ImGui::IsWindowFocused()) && (!b)) {
			panel.is_open = false;
			ImGui::End();
			return false;
		}

		if (type) {
			ImGui::InputTextMultiline("content", &prop_content, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 40));
		}

		if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
			panel.is_open = false;
			ImGui::End();
			if (panel.datatype == 0 || prop_name == NULL)
				return false;

			return true;
		}
		ImGui::End();
	}
	return false;
}

void RenderLayout(int* _loc, std::string* _name, ParaType* _type, char* add_name, bool* sel)
{
	ImGui::SetNextItemWidth(80);
	ImGui::InputInt(_name->c_str(), _loc, 1, 100, ImGuiInputTextFlags_NoName); ImGui::SameLine(); ImGui::SetNextItemWidth(60);
	if (ImGui::BeginCombo(std::to_string(*_type).c_str(), ShaderStruct::ParseType((ParaType)*_type).c_str(), ImGuiComboFlags_NoName)) {
		LOOP(ShaderStruct::type_table.size())
			if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), sel))
				*_type = (ParaType)i;
		ImGui::EndCombo();
	}ImGui::SameLine();
	_name->copy(add_name, CHAR_MAX, 0);
	*(add_name + _name->size()) = '\0'; ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputText((*_name + "name").c_str(), add_name, CHAR_MAX, ImGuiInputTextFlags_NoName | ImGuiInputTextFlags_ReadOnly);
	*_name = std::string(add_name);
}

void RenderSSBO(int* _loc, std::string* _name, ParaType* _type, Args* _args, char* add_name, bool* sel)
{
	RenderLayout(_loc, _name, _type, add_name, sel);

}

void RenderArg(Arg& _arg, int _index, bool _is_editable, bool* sel)
{
	char name[CHAR_MAX];
	std::string b_name = _arg.second;
	b_name.copy(name, b_name.size());
	*(name + b_name.size()) = '\0';
	ImGui::SetNextItemWidth(100);
	if (ImGui::BeginCombo(std::to_string(_index).c_str(), ShaderStruct::ParseType(_arg.first).c_str(), ImGuiComboFlags_NoName)) {
		LOOP(ShaderStruct::type_table.size())
			if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), sel))
				_arg.first = (ParaType)i;
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::InputTextMultiline(("m" + std::to_string(_index)).c_str(), name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20), ImGuiInputTextFlags_NoName | (!_is_editable * ImGuiInputTextFlags_ReadOnly));

	_arg.second = std::string(name);
}

void RenderArg(ParaType& _type, std::string& _name, int _index, bool _is_editable, bool* sel)
{
	char name[CHAR_MAX];
	_name.copy(name, _name.size());
	*(name + _name.size()) = '\0';
	ImGui::SetNextItemWidth(100);
	if (ImGui::BeginCombo(std::to_string(_index).c_str(), ShaderStruct::ParseType(_type).c_str(), ImGuiComboFlags_NoName)) {
		LOOP(ShaderStruct::type_table.size())
			if (ImGui::Selectable(ShaderStruct::type_table[i].c_str(), sel))
				_type = (ParaType)i;
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::InputTextMultiline(("m" + std::to_string(_index)).c_str(), name, CHAR_MAX, ImVec2(ImGui::GetContentRegionAvail().x, 20), ImGuiInputTextFlags_NoName | (!_is_editable * ImGuiInputTextFlags_ReadOnly));

	_name = std::string(name);
}

void RenderArgs(Args& args, int _type, bool* sel)
{
	int index = 0;
	for (auto& arg : args)
		RenderArg(arg, index++, _type != -1, sel);
}

bool ShaderEditor::AddParam(const char* c_name /*= ""*/, const char* c_sld_name /*= ""*/)
{

	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)) || se_panel.is_open) {
		const ImVec2 mouse_pos = ImVec2(Input::GetMousePosX(), Input::GetMousePosY());
		if (RenderPanel(se_panel, mouse_pos, &add_prop, c_name, c_sld_name)) {
			return true;
		}
	}

	return false;
}

bool ShaderEditor::AddStruct(bool def_type /*= false*/)
{
	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)) || se_panel.is_open)
	{
		const ImVec2 mouse_pos = ImVec2(Input::GetMousePosX(), Input::GetMousePosY());
		if (RenderDefPanel(se_panel, def_type, mouse_pos, &add_args)) {
			return true;
		}

	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////

void ShaderEditor::RegisterEvents(EventPool& evt)
{
	evt.subscribe<MouseClickEvent>([](MouseClickEvent e) {
		if (e.mouse == Input::MouseButtons::LMB) { // LMB
			ShaderEditor::se_node_editor.LMB();
		}
		else if (e.mouse == Input::MouseButtons::MMB) { // MMB
			if (e.key & Input::SpecialKeys::SHIFT)
				ShaderEditor::se_node_editor.SHIFT_MMB();
			else if (e.key & Input::SpecialKeys::CTRL)
				ShaderEditor::se_node_editor.CTRL_MMB();
		}
		});

	evt.subscribe<MouseScrolledEvent>([](MouseScrolledEvent e) {
		ShaderEditor::se_node_editor.SCRLL();
		});

	evt.subscribe<SelectionChangedEvent>([this](SelectionChangedEvent e) {
		this->UpdateShaderEditor(e.obj);
		});
}

void ShaderEditor::UpdateKeyword()
{
	auto& table = ShaderStruct::type_table;
	LOOP(table.size() - CUSTOM_PARA)
		if (!se_code_editor.ChechKeyword(table[i + CUSTOM_PARA]))
			se_code_editor.InsertKeyword(table[i + CUSTOM_PARA]);
}

static Shaders* GetActiveShaderPtr(ObjectID* active_obj)
{
	if (active_obj == nullptr)
		return nullptr;

	return (Shaders*)(active_obj->GetShader());
}

static Shaders::ShaderUnit* GetShaderUnitPtr(Shaders* shader, ShaderType tar)
{
	if (shader == nullptr) return nullptr;

	return shader->GetShaderUnit(tar);
}

void ShaderEditor::RenderName(const std::string& _label, std::string* _name, float _width, bool read_only) const
{
	RenderName((_label + *_name).c_str(), _name, _width, read_only);
}

void ShaderEditor::RenderName(const char* _label, std::string* _name, float _width, bool read_only /*= true*/) const
{
	char name[CHAR_MAX];
	std::string temp = *_name;
	//DEBUG(_name->size())
	temp.copy(name, temp.size());
	*(name + temp.size()) = '\0';
	ImGui::InputTextMultiline(_label, name, CHAR_MAX, ImVec2(_width == 0.0f ? ImGui::GetContentRegionAvail().x : _width, 20), ImGuiInputTextFlags_NoName | (read_only ? ImGuiInputTextFlags_ReadOnly : 0));

	if (!read_only)
		*_name = std::string(name);
}

void ShaderEditor::RenderShaderStruct(ObjectID* active_obj)
{
	Shaders* shader = GetActiveShaderPtr(active_obj);
	Shaders::ShaderUnit* active_unit = GetShaderUnitPtr(shader, (ShaderType)current_shad_type);

	if (active_unit == nullptr) return;
	if (!active_unit->sh_struct.has_value()) return;

	int type_id = 0, vari_id = 0;
	// [BASE INFO]
	ImGui::PushID(type_id++);

	if (ImGui::TreeNode("Base Information")) {
		ImGui::Text("========================");
		ImGui::Text("GLSL version : %i", active_unit->sh_struct->version);
		ImGui::Text("Shader Type : %s", Shaders::shader_type[current_shad_type].c_str());
		ImGui::Text("Shader ID : %i", active_unit->sh_ID);
		ImGui::Text(active_unit->sh_struct->is_struct_changed ? "Status : Changed" : "Status : Compiled");
		ImGui::Text("========================");
		ImGui::TreePop();
	}
	ImGui::PopID();
	// [AB]
	if (active_unit->sh_struct->AB_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Array Buffer")) {
			for (auto& l_in : active_unit->sh_struct->AB_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(l_in).c_str())) {
					RenderLayout(&std::get<0>(l_in), &std::get<1>(l_in), &std::get<2>(l_in), add_name, &sel);
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddParam("Array Buffer", "layout")) {
					active_unit->sh_struct->SetAB(std::get<2>(add_prop), std::get<1>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
		}ImGui::PopID(); vari_id = 0;
	}

	// [PASS]
	if (active_unit->sh_struct->pass_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Rendering Pass")) {
			for (auto& i : active_unit->sh_struct->pass_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddParam("Render Pass", "layout")) {
					active_unit->sh_struct->SetPass(std::get<2>(add_prop), std::get<1>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
		}ImGui::PopID(); vari_id = 0;
	}

	//[IN]
	if (active_unit->sh_struct->input_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Input")) {
			for (auto& i : active_unit->sh_struct->input_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddParam("Input", "number")) {
				active_unit->sh_struct->SetInp(std::get<1>(add_prop), std::get<2>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
		}ImGui::PopID(); vari_id = 0;
	}

	//[OUT]
	if (active_unit->sh_struct->output_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Output")) {
			for (auto& i : active_unit->sh_struct->output_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddParam("Output", "number")) {
				active_unit->sh_struct->SetOut(std::get<1>(add_prop), std::get<2>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
		}ImGui::PopID(); vari_id = 0;
	}

	//[UNIFORM]
	if (active_unit->sh_struct->uniform_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Uniform")) {
			int index = 0;
			for (auto& i : active_unit->sh_struct->uniform_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					RenderArg(std::get<1>(i), std::get<0>(i), index++, false, &sel);
					ImGui::TreePop();
				}ImGui::PopID();
			}
			//ImGui::PushFont(ImguiTheme::th_data.font_data[0]);
			if (AddParam("Uniforms", "number")) {
					active_unit->sh_struct->SetUni(std::get<1>(add_prop), std::get<2>(add_prop), std::get<0>(add_prop));
			}ImGui::TreePop();
			//ImGui::PopFont();
		}ImGui::PopID(); vari_id = 0;
	}

	//[STRUCT]
	if (active_unit->sh_struct->struct_def_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Struct")) {
			for (auto& i : active_unit->sh_struct->struct_def_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					RenderName("struct name", &std::get<1>(i));
					RenderArgs(std::get<2>(i), 0, &sel);
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (AddStruct()) {
					active_unit->sh_struct->DefStruct(std::get<1>(add_args), std::get<3>(add_args));
			}ImGui::TreePop();
		}ImGui::PopID(); vari_id = 0;
	}

	//[Globs]
	if (active_unit->sh_struct->glob_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Globs")) {
			for (auto& i : active_unit->sh_struct->glob_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{
				// TODO
			}ImGui::TreePop();
		}ImGui::PopID(); vari_id = 0;
	}

	//[Const]
	if (active_unit->sh_struct->const_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Consts")) {
			for (auto& i : active_unit->sh_struct->const_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					RenderName(("name_" + std::get<1>(i)).c_str(), &std::get<1>(i), 100);
					ImGui::SameLine();
					RenderName(("content" + std::get<1>(i)).c_str(), &std::get<2>(i), 200, false);

					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{
				// TODO
			}ImGui::TreePop();
		}ImGui::PopID(); vari_id = 0;
	}

	//[Funcs]
	if (active_unit->sh_struct->func_list.size()) {
		ImGui::PushID(type_id++);
		if (ImGui::TreeNode("Functions")) {
			for (auto& i : active_unit->sh_struct->func_list) {
				bool op_ev = false, st_ev = false;
				ImGui::PushID(vari_id++);
				bool is_op = ImGui::TreeNodeB(
					std::get<1>(i).c_str(),
					&active_unit->sh_struct->func_list_state[vari_id - 1],
					&op_ev,
					&st_ev,
					0,
					(active_func != vari_id)
				);
				if (op_ev) {
					se_code_editor.SetText(std::get<2>(i));
					active_func = vari_id;
				}
				if (active_func == vari_id && is_op) {
					RenderName("func name", &std::get<1>(i));

					if (ImGui::TreeNode("arguments list")) {
						RenderArgs(std::get<3>(i), -1, &sel);
						ImGui::TreePop();
					}
					if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x, 20))) {
						std::get<2>(i) = se_code_editor.GetText();
					}
					se_code_editor.Render(std::get<1>(i).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 100), true);
				}ImGui::PopID();
				if (is_op)ImGui::TreePop();
				if (st_ev) {
					if (se_code_editor.IsTextChanged())
						std::get<2>(i) = se_code_editor.GetText();
					active_unit->sh_struct->is_struct_changed = true;
				}
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{
				// TODO
			}ImGui::TreePop();
		}ImGui::PopID(); vari_id = 0;
	}

	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 5, 20)))
	{
		// TODO
	}
}

void ShaderEditor::UpdateShaderEditor(ObjectID* obj) const {

	Shaders* shader = GetActiveShaderPtr(obj);
	Shaders::ShaderUnit* active_unit = GetShaderUnitPtr(shader, (ShaderType)current_shad_type);

	if (active_unit == nullptr) return;

	if (current_edit == ShaderEditor::CODE_EDITOR) {
		se_code_editor.SetText(active_unit->sh_code);
	}
	UpdateKeyword();
}

bool ShaderEditor::CompileShader(ObjectID* active_obj)
{
	Shaders* active_shader = GetActiveShaderPtr(active_obj);
	Shaders::ShaderUnit* active_unit = GetShaderUnitPtr(active_shader, (ShaderType)current_shad_type);

	if (active_unit == nullptr || active_shader == nullptr) return false;

	Timer timer;
	switch (current_edit) {
	case CODE_EDITOR:
		if (se_code_editor.GetText() == active_unit->sh_code) return false;
		active_shader->ParseShaderCode(se_code_editor.GetText(), (ShaderType)current_shad_type);
		break;
	case STRUCT_EDITOR:
		if (!active_unit->sh_struct->is_struct_changed) return false;
		active_shader->GenerateShader((ShaderType)current_shad_type);
		break;
	}

	active_shader->RelinkShader((ShaderType)current_shad_type);
	return true;
}

void ShaderEditor::RenderLayer(const SceneContext& ctx, const EventPool& evt)
{
	ObjectID* active_obj = ctx.c_selections.GetSelectedObjects();
	Shaders* active_shader = GetActiveShaderPtr(active_obj);
	Shaders::ShaderUnit* active_unit = GetShaderUnitPtr(active_shader, (ShaderType)current_shad_type);
	if (active_shader == nullptr) {
		ImGui::Text("No active shader");
		return;
	}
	bool is_mode_changed = false, is_shad_type_changed = false, is_shader_changed = false;

	if (ImGui::BeginCombo("Edit Mode", edit_mode[current_edit].c_str())) {
		LOOP(3)
			if (ImGui::Selectable(edit_mode[i].c_str(), &sel)) {
				is_mode_changed |= current_edit != i;
				current_edit = ShaderEditMode(i);
			}

		ImGui::EndCombo();
	}
	if (ImGui::BeginCombo("Shader Type", Shaders::shader_type[current_shad_type].c_str())) {
		const std::vector<ShaderType> all_types = active_shader->GetAllShaderTypes();
		LOOP(all_types.size()) {
			const ShaderType type = all_types[i];
			if (ImGui::Selectable(Shaders::shader_type[type].c_str(), &sel)) {
				is_shad_type_changed |= current_shad_type != type;
				current_shad_type = type;
			}
		}

		ImGui::EndCombo();
	}
	if (ImGui::Button("Compile", ImVec2(ImGui::GetContentRegionAvail().x / 2, 25))) {
		is_shader_changed = CompileShader(active_obj);
	}ImGui::SameLine();
	if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 25))) {
		//active_shader->GenerateShader((ShaderType)current_shad_type);
		//DEBUG(active_shader->getID())
	}
	//editor.SetLanguageDefinition(TextEditor::LanguageDefinition().GLSL());

	switch (current_edit)
	{
	case CODE_EDITOR:
		if (active_unit) {
			se_code_editor.Render("##Editor", ImGui::GetContentRegionAvail());
		}

		break;
	case STRUCT_EDITOR:
		RenderShaderStruct(active_obj);
		break;
	case NODE_EDITOR:
		se_node_editor.Render(ctx, "##Node");
		break;
	}

	if (is_shad_type_changed || is_mode_changed || (is_shader_changed && current_edit != ShaderEditor::CODE_EDITOR)) {
		UpdateShaderEditor(active_obj);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
