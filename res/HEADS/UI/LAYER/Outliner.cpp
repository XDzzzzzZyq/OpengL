#include "Outliner.h"

Outliner::Outliner()
{
	uly_type = OUTLINER_UILAYER;
	uly_name = "Outliner";
}

Outliner::Outliner(const std::string& name)
{
	uly_type = OUTLINER_UILAYER;
	uly_name = name;

	ImGui::GetIO().Fonts->AddFontDefault();
	fontA = ImGui::GetIO().Fonts->AddFontFromFileTTF("res/fonts/GenJyuuGothicX-Bold.ttf", 18.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
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
		PushItem<UI::OpaButton>(ol_data[i].NAME, ol_data[i].ID);
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
	if (EventListener::is_GOlist_changed) {
		SetObjectList(&EventListener::outline_list);
	}
}

void Outliner::RenderLayer() const
{
	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		ImVec2 p = ImGui::GetCursorScreenPos() - ImVec2(0, 2);
		GetLayerSize();
		LOOP(round(uly_size.y / (ol_width * 2)) + 1) {
			float frac = i == round(uly_size.y / (ol_width * 2)) ? uly_size.y - (2 * i + 1) * ol_width : 0;
			frac = frac < -ol_width ? -ol_width : frac;
			ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(0, i * ol_width * 2), p + ImVec2(uly_size.x, (2 * i + 1) * ol_width + frac), IM_COL32(255, 255, 255, 20));
		}

		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.1f, 0.5f));
		ImGui::PushFont(fontA);
		for (auto& item : item_list) {
			item->RenderItem();
		}
		ImGui::PopFont();
		ImGui::PopStyleVar();


		LOOP(item_list.size()) {
			if (item_list[i]->is_activated) {
				if (i != actIndex) {
					EventListener::is_selected_changed = true;
					EventListener::pre_act_go_ID = EventListener::active_GO_ID;
					EventListener::active_GO_ID = index2id[i];

					EventListener::is_outliner_selected = true;
				}
			}

		}

		if (EventListener::is_selected_changed) {

			// deselect previous object
			if (EventListener::pre_act_go_ID != 0)
				FindImguiItem(id2index[EventListener::pre_act_go_ID])->is_activated = false;

			// select new objects
			if (EventListener::active_GO_ID != 0) {
				actIndex = id2index[EventListener::active_GO_ID];
				EventListener::active_object = EventListener::GetActiveObject(active_GO_ID);
				FindImguiItem(actIndex)->is_activated = true;
			}

		}

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
