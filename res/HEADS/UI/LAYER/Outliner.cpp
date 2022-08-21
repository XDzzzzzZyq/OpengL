#include "Outliner.h"

Outliner::Outliner()
{
	uly_name = "Outliner";
}

Outliner::Outliner(const std::string& name)
{
	uly_name = name;
}

Outliner::~Outliner()
{

}

void Outliner::SetObjectList(OutlineData* data)
{
	item_list.clear();
	ImVec2 size = ImVec2(uly_size.x, ol_width - 4);
	ol_data = *data;
	actIndex = -1;
	LOOP(ol_data.size())
	{
		PushItem(new UI::OpaButton(ol_data[i].NAME, ol_data[i].ID));
		id2index[ol_data[i].ID] = item_list.size() - 1;
		index2id[item_list.size() - 1] = ol_data[i].ID;
		item_list[item_list.size() - 1]->ResetSize(size);
	}
}

void Outliner::UpdateStyle()
{

}

void Outliner::UpdateLayer()
{
	if (is_GOlist_changed) {
		SetObjectList(&outline_list);
	}

	LOOP(item_list.size()) {
		if (item_list[i]->is_activated) {
			if (i != actIndex) {
				is_selected_changed = true;
				pre_act_go_ID = active_GO_ID;
				active_GO_ID = index2id[i];

				is_outliner_selected = true;
			}
		}

	}

	if (is_selected_changed) {
		if (pre_act_go_ID != 0)
			FindImguiItem(id2index[pre_act_go_ID])->is_activated = false;
		if (active_GO_ID != 0) {
			actIndex = id2index[active_GO_ID];
			FindImguiItem(actIndex)->is_activated = true;
		}

	}



}

void Outliner::RenderLayer() const
{
	if (!uly_is_rendered)
		return;

	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		ImVec2 p = ImGui::GetCursorScreenPos() - ImVec2(0, 2);
		GetLayerSize();
		LOOP(round(uly_size.y / (ol_width * 2)) + 1) {
			float frac = i == round(uly_size.y / (ol_width * 2)) ? uly_size.y - (2 * i + 1) * ol_width : 0;
			frac = frac < -ol_width ? -ol_width : frac;
			ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(0, i * ol_width * 2), p + ImVec2(uly_size.x, (2 * i + 1) * ol_width + frac), IM_COL32(255, 255, 255, 20));
		}

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.1f, 0.5f));
		for (auto& item : item_list) {
			item->RenderItem();
		}
		ImGui::PopStyleVar();

		if (is_size_changed)
			is_size_changed_b = is_size_changed;

		is_size_changed = false;
		ImGui::End();
	}
	else {
		uly_is_rendered = false;
		ImGui::End();
	}

}
