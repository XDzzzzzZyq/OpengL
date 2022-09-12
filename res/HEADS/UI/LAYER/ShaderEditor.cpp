#include "ShaderEditor.h"

std::string ShaderEditor::edit_mode[3] = { "Shader Code", "Hierarchy", "Nodes" };

std::string ShaderEditor::shader_type[2] = { "Vertex Shader", "Fragment Shader" };

ShaderEditor::ShaderEditor()
{
	uly_name = "Shader Editor";
}

ShaderEditor::ShaderEditor(const std::string& name)
{
	uly_name = name;
}

ShaderEditor::~ShaderEditor()
{

}

void ShaderEditor::UpdateLayer()
{
	if (is_selected_changed) {
		UpdateShaderEditor();
	}

}

void ShaderEditor::RenderShaderStruct() const
{
	int type_id = 0, vari_id = 0;
	// [BASE INFO]
	ImGui::PushID(type_id);

	if (ImGui::TreeNode("Base Information")) {
		ImGui::Text("GLSL version : %i", active_shader->shader_struct_list[current_shad_type].version);
		ImGui::Text("Shader Type : " + current_shad_type == 0 ? "Vertex Shader" : "Fragment Shader");
		ImGui::Text("Status : Compiled");
		ImGui::TreePop();
	}
	ImGui::PopID();type_id++;

	// [AB]
	if (active_shader->shader_struct_list[current_shad_type].AB_list.size()) {
		ImGui::PushID(type_id);
		if (ImGui::TreeNode("Array Buffer")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].AB_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x-15, 20)))
			{
				
			}ImGui::TreePop();
		}ImGui::PopID();type_id++;vari_id = 0;  
	}

	// [PASS]
	if (active_shader->shader_struct_list[current_shad_type].pass_list.size()) {
		ImGui::PushID(type_id);
		if (ImGui::TreeNode("Rendering Pass")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].pass_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{

			}ImGui::TreePop();
		}ImGui::PopID();type_id++;vari_id = 0;
	}

	//[IN]
	if (active_shader->shader_struct_list[current_shad_type].input_list.size()) {
		ImGui::PushID(type_id);
		if (ImGui::TreeNode("Input")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].input_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{

			}ImGui::TreePop();
		}ImGui::PopID();type_id++;vari_id = 0;
	}

	//[OUT]
	if (active_shader->shader_struct_list[current_shad_type].output_list.size()) {
		ImGui::PushID(type_id);
		if (ImGui::TreeNode("Output")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].output_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{

			}ImGui::TreePop();
		}ImGui::PopID();type_id++;vari_id = 0;
	}

	//[UNIFORM]
	if (active_shader->shader_struct_list[current_shad_type].uniform_list.size()) {
		ImGui::PushID(type_id);
		if (ImGui::TreeNode("Uniform")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].uniform_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<0>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{
				active_shader->shader_struct_list[current_shad_type].SetUni(FLOAT_PARA, 1, "TEST_UNIFORM");
			}ImGui::TreePop();
		}ImGui::PopID();type_id++;vari_id = 0;

		//DEBUG(active_shader->shader_struct_list[current_shad_type].is_struct_changed)
	}
	
	//[STRUCT]
	if (active_shader->shader_struct_list[current_shad_type].struct_def_list.size()) {
		ImGui::PushID(type_id);
		if (ImGui::TreeNode("Struct")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].struct_def_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{

			}ImGui::TreePop();
		}ImGui::PopID();type_id++;vari_id = 0;
	}

	//[Globs]
	if (active_shader->shader_struct_list[current_shad_type].glob_list.size()) {
		ImGui::PushID(type_id);
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
		}ImGui::PopID();type_id++;vari_id = 0;
	}

	//[Const]
	if (active_shader->shader_struct_list[current_shad_type].const_list.size()) {
		ImGui::PushID(type_id);
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
		}ImGui::PopID();type_id++;vari_id = 0;
	}

	//[Funcs]
	if (active_shader->shader_struct_list[current_shad_type].func_list.size()) {
		ImGui::PushID(type_id);
		if (ImGui::TreeNode("Functions")) {
			for (auto& i : active_shader->shader_struct_list[current_shad_type].func_list) {
				ImGui::PushID(vari_id);
				if (ImGui::TreeNode(std::get<1>(i).c_str())) {
					ImGui::TreePop();
				}ImGui::PopID();
			}
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 15, 20)))
			{

			}ImGui::TreePop();
		}ImGui::PopID();type_id++;vari_id = 0;
	}

	if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x - 5, 20)))
	{

	}
}

void ShaderEditor::UpdateShaderEditor() {

}

void ShaderEditor::RenderLayer() const
{
	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {


		if (ImGui::BeginCombo("Edit Mode", edit_mode[current_edit].c_str())) {
			LOOP(3)
				if (ImGui::Selectable(edit_mode[i].c_str(), &sel))
					current_edit = i;
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Shader Type", shader_type[current_shad_type].c_str())) {
			LOOP(2)
				if (ImGui::Selectable(shader_type[i].c_str(), &sel))
					current_shad_type = i;
			ImGui::EndCombo();
		}
		if (ImGui::Button("Compile", ImVec2(ImGui::GetContentRegionAvail().x/2, 25))) {
			active_shader->GenerateShader((ShaderType)current_shad_type);
			active_shader->CompileShader((ShaderType)current_shad_type);
		}ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 25))) {
			//active_shader->GenerateShader((ShaderType)current_shad_type);
			//DEBUG(active_shader->getID())
		}

		switch (current_edit)
		{
		case 0:
			if (active_shader)
				ImGui::Text(active_shader->shader_list[current_shad_type].c_str());
			break;
		case 1:
			if (active_shader)
				RenderShaderStruct();
			break;
		case 2:
			break;
		}

		//if(active_shader)
			//DEBUG(1212)
		ImGui::End();
	}
}