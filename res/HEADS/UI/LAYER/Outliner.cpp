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
		if (ol_data[i].ID == actID) {
			PushItem(new UI::OpaButton("[ " + ol_data[i].NAME + " ]"));
			actIndex = i;
		}
		else {
			PushItem(new UI::OpaButton(ol_data[i].NAME));
		}

		
		item_list[item_list.size()-1]->ResetSize(size);
	}
}

void Outliner::SetActiveData(int ID)
{
	actID = ID;
}

void Outliner::UpdateStyle()
{

}

void Outliner::RenderLayer() const
{
	if (!uly_is_rendered)
		return;

	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		ImVec2 p = ImGui::GetCursorScreenPos()-ImVec2(0,2);
		GetLayerSize();
		//if(item_list[0])
		//	ol_width = item_list[0]->GetPara()->para_data.idata;
		LOOP(round(uly_size.y / (ol_width * 2)) + 1) {
			float frac = i == round(uly_size.y / (ol_width * 2)) ? uly_size.y - (2 * i + 1) * ol_width : 0;
			frac = frac < -ol_width ? -ol_width : frac;
			ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(0, i * ol_width * 2), p + ImVec2(uly_size.x, (2 * i + 1) * ol_width + frac), IM_COL32(255, 255, 255, 20));
		}

		int i = 0;
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.1f, 0.5f));
		for (auto& item : item_list) {
			if (i == actIndex) {
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
				item->RenderItem();
				ImGui::PopStyleVar();
			}
			else
			{
				item->RenderItem();
			}

			i++;
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
