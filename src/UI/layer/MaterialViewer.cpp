#include "MaterialViewer.h"
#include "Material.h"

#include "events/MaterialEvents.h"

#include <algorithm>
#include <array>
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

static Material* GetActiveMatPtr(const Context& ctx)
{
	ObjectID* active_object = ctx.editor.selections.GetSelectedObjects();
	if (active_object == nullptr)
		return nullptr;

	return (Material*)(active_object->GetMaterial());
}

struct MaterialParamEdit
{
	Material::MatDataType data_type;
	float value;
	glm::vec3 color;
	std::string texture_name;
};

static std::optional<MaterialParamEdit> EditMatFloat(const char* name, float value)
{
	if (!ImGui::SliderFloat(name, &value, 0, 1))
		return std::nullopt;

	return MaterialParamEdit{ Material::MPARA_FLT, value, glm::vec3(0.0f), {} };
}

static std::optional<MaterialParamEdit> EditMatColor(const char* name, glm::vec3 color)
{
	if (!ImGui::ColorEdit3(name, (float*)&color))
		return std::nullopt;

	return MaterialParamEdit{ Material::MPARA_COL, 0.0f, color, {} };
}

static std::optional<MaterialParamEdit> EditMatTexture(const char* name, const std::string& texture_name)
{
	ImGui::InputText(name, (char*)texture_name.c_str(), CHAR_MAX, ImGuiInputTextFlags_ReadOnly);
	return std::nullopt;
}

bool MaterialViewer::RenderName(const std::string& current_name, std::string& out_name, bool read_only /*= false*/)
{
	std::array<char, CHAR_MAX> name_buf{};
	const auto name_len = std::min(current_name.size(), name_buf.size() - 1);
	std::copy_n(current_name.data(), name_len, name_buf.data());
	name_buf[name_len] = '\0';
	ImGui::InputText("Name", name_buf.data(), static_cast<int>(name_buf.size()), ImGuiInputTextFlags_NoHorizontalScroll | (read_only ? ImGuiInputTextFlags_ReadOnly : 0));
	out_name = std::string(name_buf.data());

	ImGui::NewLine();
	return !read_only && out_name != current_name;
}

void MaterialViewer::RenderLayer(const Context& ctx, const EventPool& evt)
{
	Material* active_material = GetActiveMatPtr(ctx);
	if (active_material == nullptr) {
		ImGui::Text("No selected material");
		return;
	}

	//  Material Preview

	//  Material Name
	std::string new_name;
	if (RenderName(active_material->mat_name, new_name)) {
		evt.emit<MaterialNameChangedEvent>({ active_material, new_name });
	}

	//  Material Parameters
	for (auto & [type, param] : active_material->mat_params) {
		const auto& [data_type, value, color, texture] = param;
		const char* pname = Material::mat_uniform_name[type].c_str();
		std::optional<MaterialParamEdit> edit;
		switch (data_type)
		{
		case Material::MPARA_FLT:
			edit = EditMatFloat(pname, value);
			if (edit)
				evt.emit<MaterialFloatChangedEvent>({ active_material, type, edit->data_type, edit->value });
			break;
		case Material::MPARA_COL:
			edit = EditMatColor(pname, color);
			if (edit)
				evt.emit<MaterialColorChangedEvent>({ active_material, type, edit->data_type, edit->color });
			break;
		case Material::MPARA_TEX:
			edit = EditMatTexture(pname, texture ? texture->GetTexName() : std::string());
			if (edit)
				evt.emit<MaterialTextureNameChangedEvent>({ active_material, type, edit->data_type, edit->texture_name });
			break;
		default:
			break;
		}
	}
}
