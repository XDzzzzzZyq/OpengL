#include "MaterialViewer.h"
#include "Material.h"

#include "events/MaterialEvents.h"

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

static bool RenderMatParam(MatParaType _type, Material::MatParamData& _param)
{
	// TODO: change the type of variable
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

void MaterialViewer::RenderLayer(const Context& ctx, const EventPool& evt)
{
	Material* active_material = GetActiveMatPtr(ctx);
	if (active_material == nullptr) {
		ImGui::Text("No selected material");
		return;
	}

	//  Material Preview

	//  Material Name
	RenderName(active_material->mat_name);

	//  Material Parameters
	bool is_material_changed = false;
	for (auto & [type, param] : active_material->mat_params) {
		is_material_changed |= RenderMatParam(type, param);
	}

	if (is_material_changed) {
		evt.emit<MaterialChangedEvent>({ active_material });
	}
}
