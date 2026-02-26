#include "MaterialViewer.h"
#include "Material.h"

#include "events/MaterialEvents.h"

#include <optional>

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

Material* GetActiveMatPtr(ObjectID* obj)
{
	if (obj == nullptr)
		return nullptr;

	return (Material*)(obj->GetMaterial());
}

std::optional<Material::MatDataType> SelectType(Material::MatParaType tar, Material::MatDataType type) {
	static std::string type_name[3] = {"float", "vec3", "tex"};
	std::optional<Material::MatDataType> new_type = std::nullopt;

	ImGui::SetNextItemWidth(75);
	if (ImGui::BeginCombo(("m_tar" + std::to_string(tar)).c_str(), type_name[type].c_str(), ImGuiComboFlags_NoName)) {
		LOOP(3) {
			bool sel = (i == type);
			if (ImGui::Selectable(type_name[i].c_str(), &sel))
				new_type = Material::MatDataType(i);
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine(100.0f, 0.0);

	return new_type;
}

struct MaterialParamEdit
{
	Material::MatDataType data_type;
	float value;
	glm::vec3 color;
	std::string texture_name;
};

std::optional<MaterialParamEdit> EditMatFloat(const char* name, float value)
{
	if (!ImGui::SliderFloat(name, &value, 0, 1))
		return std::nullopt;

	return MaterialParamEdit{ Material::MPARA_FLT, value, glm::vec3(0.0f), {} };
}

std::optional<MaterialParamEdit> EditMatColor(const char* name, glm::vec3 color)
{
	if (!ImGui::ColorEdit3(name, (float*)&color))
		return std::nullopt;

	return MaterialParamEdit{ Material::MPARA_COL, 0.0f, color, {} };
}

std::optional<MaterialParamEdit> EditMatTexture(const char* name, const std::string& texture_name)
{
	ImGui::InputText(name, (char*)texture_name.c_str(), CHAR_MAX, ImGuiInputTextFlags_ReadOnly);
	return std::nullopt;
}

bool EditName(std::string& name)
{
	const std::string current_name = name;
	name.resize(CHAR_MAX - 1);
	ImGui::InputText("Name", name.data(), static_cast<int>(name.size()), ImGuiInputTextFlags_NoHorizontalScroll);
	name = std::string(name.c_str());

	ImGui::NewLine();
	return name != current_name;
}

void MaterialViewer::RenderLayer(const Context& ctx, EventPool& evt)
{
	ObjectID* active_object = ctx.editor.selections.GetSelectedObjects();
	Material* active_material = GetActiveMatPtr(active_object);
	if (active_material == nullptr) {
		ImGui::Text("No selected material");
		return;
	}

	//  Material Preview

	//  Material Name
	std::string new_name = active_material->mat_name;
	if (EditName(new_name)) {
		evt.emit<MaterialNameChangedEvent>({ active_material, new_name });
	}

	//  Material Parameters
	for (auto & [prop, param] : active_material->mat_params) {
		const auto& [data_type, value, color, texture] = param;
		const char* pname = Material::mat_uniform_name[prop].c_str();

		const std::optional<Material::MatDataType> new_type = SelectType(prop, data_type);
		if (new_type && *new_type != data_type) {
			evt.emit<MaterialTypeChangedEvent>({ active_object, active_material, prop, *new_type });
		}

		std::optional<MaterialParamEdit> edit;
		switch (data_type)
		{
		case Material::MPARA_FLT:
			edit = EditMatFloat(pname, value);
			if (edit)
				evt.emit<MaterialFloatChangedEvent>({ active_material, prop, edit->data_type, edit->value });
			break;
		case Material::MPARA_COL:
			edit = EditMatColor(pname, color);
			if (edit)
				evt.emit<MaterialColorChangedEvent>({ active_material, prop, edit->data_type, edit->color });
			break;
		case Material::MPARA_TEX:
			edit = EditMatTexture(pname, texture ? texture->GetTexName() : std::string());
			if (edit)
				evt.emit<MaterialTextureNameChangedEvent>({ active_material, prop, edit->data_type, edit->texture_name });
			break;
		default:
			break;
		}
	}
}
