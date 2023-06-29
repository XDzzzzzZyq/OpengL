#include "TransformPanel.h"

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

Transform* TransformPanel::GetActiveTransPtr()
{
	if (EventListener::active_object == nullptr)
		return nullptr;

	return (Transform*)(EventListener::active_object->GetTransform());
}

bool TransformPanel::RenderTransfroms(Transform3D& trans)
{
	glm::vec3 pos = trans.o_position;
	glm::vec3 rot = trans.o_rot;
	glm::vec3 scl = trans.o_scale;

	const bool is_pos_ch = ImGui::InputFloat3("Position", (float*)&pos, "%.1f");
	const bool is_rot_ch = ImGui::InputFloat3("Rotation", (float*)&rot, "%.1f");
	const bool is_scl_ch = ImGui::InputFloat3("Scale",	  (float*)&scl, "%.1f");

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
	Transform3D* active_trans = dynamic_cast<Transform3D*>(TransformPanel::GetActiveTransPtr());

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
