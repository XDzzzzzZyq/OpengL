#include "TransformPanel.h"
#include "Transform.h"

TransformPanel::TransformPanel()
	:TransformPanel("Transform")
{}

TransformPanel::TransformPanel(std::string _name)
{
	uly_type = TRANSFORM_UILAYER;
	uly_name = _name;
}

TransformPanel::~TransformPanel()
{

}

static Transform* GetActiveTransPtr()
{
	if (EventListener::active_object == nullptr)
		return nullptr;

	return (Transform*)(EventListener::active_object->GetTransform());
}

ImGuiInputTextFlags _SliderFlag(bool _is_locked) {
	return _is_locked ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_AutoSelectAll;
}

static bool RenderTransfroms(Transform3D& trans)
{
	glm::vec3 pos = trans.o_position;
	glm::vec3 rot = trans.o_rot;
	glm::vec3 scl = trans.o_scale;
	std::string id = std::to_string((int)trans.GetTransformPtr());

	const bool is_pos_ch = ImGui::InputFloat3M((id + "p").c_str(), "Position", (float*)&pos, "%.1f", ::_SliderFlag(!trans.use_position));
	ImGui::SameLine(); ImGui::Checkbox((id + "pl").c_str(), &trans.use_position, false);
	const bool is_rot_ch = ImGui::InputFloat3M((id + "r").c_str(), "Rotation", (float*)&rot, "%.1f", ::_SliderFlag(!trans.use_position));
	ImGui::SameLine(); ImGui::Checkbox((id + "rl").c_str(), &trans.use_rotation, false);
	const bool is_scl_ch = ImGui::InputFloat3M((id + "s").c_str(), "Scale   ", (float*)&scl, "%.1f", ::_SliderFlag(!trans.use_position));
	ImGui::SameLine(); ImGui::Checkbox((id + "sl").c_str(), &trans.use_scale, false);

	if (is_pos_ch)
		trans.SetPos(pos);
	if (is_rot_ch)
		trans.SetRot(rot);
	if (is_scl_ch)
		trans.SetScale(scl);

	return is_pos_ch || is_rot_ch || is_scl_ch;
}

void TransformPanel::RenderLayer()
{
	Transform3D* active_trans = dynamic_cast<Transform3D*>(GetActiveTransPtr());

	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		if (active_trans == nullptr) {
			ImGui::Text("No selected transform component");
			ImGui::End();
			return;
		}
		ImGui::InputText("Name", (char*)EventListener::active_object->o_name.c_str(), CHAR_MAX, ImGuiInputTextFlags_ReadOnly);

		RenderTransfroms(*active_trans);

	}
	ImGui::End();
}
