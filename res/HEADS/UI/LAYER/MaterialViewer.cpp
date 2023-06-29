#include "MaterialViewer.h"

MaterialViewer::MaterialViewer()
	:MaterialViewer("Material")
{}

MaterialViewer::MaterialViewer(std::string _name)
{
	uly_type = MATERIAL_UILYER;
	uly_name = _name;
}

MaterialViewer::~MaterialViewer()
{

}

Material* MaterialViewer::GetActiveMatPtr()
{
	if (EventListener::active_object == nullptr)
		return nullptr;

	return (Material*)(EventListener::active_object->GetMaterial());
}

void MaterialViewer::RenderName(std::string& _name, bool read_only /*= false*/)
{
	static char name[CHAR_MAX];
	_name.copy(name, _name.size());
	*(name + _name.size()) = '\0';
	ImGui::InputText("Name", name, CHAR_MAX, ImGuiInputTextFlags_NoHorizontalScroll | (read_only ? ImGuiInputTextFlags_ReadOnly : 0));

	if (!read_only)
		_name = std::string(name);

	ImGui::NewLine();
}

void MaterialViewer::RenderLayer()
{
	Material* active_material = MaterialViewer::GetActiveMatPtr();

	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		if (active_material == nullptr) {
			ImGui::Text("No selected material");
			ImGui::End();
			return;
		}

		//  Material Preview

		//  Material Name
		RenderName(active_material->mat_name);

		//  Material Parameters
		for (auto & [type, param] : active_material->mat_params) {
			active_material->is_mat_changed |= RenderMatParam(type, param);
		}

	}
	ImGui::End();
}

bool MaterialViewer::RenderMatParam(MatParaType _type, Material::MatParamData& _param)
{
	auto& [type, val, col, tex] = _param;

	const char* pname = Material::mat_uniform_name[_type].c_str();
	bool is_changed = false;

	switch (type)
	{
	case Material::MPARA_FLT:
		is_changed = ImGui::SliderFloat(pname, &val, 0, 1);
		break;
	case Material::MPARA_COL:
		is_changed = ImGui::ColorEdit3(pname, (float*)&col);
		break;
	case Material::MPARA_TEX:
		ImGui::InputText(pname, (char*)tex->GetTexName().c_str(), CHAR_MAX, ImGuiInputTextFlags_ReadOnly);
		break;
	}

	return is_changed;
}
