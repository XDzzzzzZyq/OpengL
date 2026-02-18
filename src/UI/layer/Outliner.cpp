#include "Outliner.h"
#include "Input.h"

#include "item/OpaButton.h"

#include "events/EditorEvents.h"

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

void Outliner::UpdateStyle()
{

}

void Outliner::RenderLayer(const Context& ctx, const EventPool& evt)
{
	const SelectionManager<ObjectID>& sel_mgr = ctx.editor.selections;
	const auto obj_list = ctx.scene.GetObjectIDs();

	const ImVec2 p = ImGui::GetCursorScreenPos() - ImVec2(0, 2);
	const ImVec2 size = ImVec2(uly_size.x, float(ol_width - 4));

	LOOP(round(uly_size.y / (ol_width * 2)) + 1) {
		float frac = i == round(uly_size.y / (ol_width * 2)) ? uly_size.y - (2 * i + 1) * ol_width : 0;
		frac = frac < -ol_width ? -ol_width : frac;
		ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(0.0, float(i * ol_width * 2)), p + ImVec2(uly_size.x, (2 * i + 1) * ol_width + frac), IM_COL32(255, 255, 255, 20));
	}

	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.1f, 0.5f));
	ImGui::PushFont(fontA);

	for (const ObjectID* obj : obj_list) {
		const bool selected = sel_mgr.IsSelected((ObjectID*)obj);

		if (selected) ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1, 0.5, 0, 1);

		bool is_button_pressed = ImGui::Selectable(obj->o_name.c_str(), &is_button_pressed, 0, size);

		if (selected) ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);

		if (is_button_pressed) {
			evt.emit(ObjectSelectedEvent(obj->GetObjectID(), Input::IsKeyPressed(Input::SHIFT)));
		}
	}

	ImGui::PopFont();
	ImGui::PopStyleVar();
}
