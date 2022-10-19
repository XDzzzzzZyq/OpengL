#include "NodeEditor.h"

NodeEditor::NodeEditor()
	:NodeEditor(NONE_NODE_EDITOR)
{
}

NodeEditor::NodeEditor(NodeEditorType type)
	:_type(type)
{
	EventList[GenIntEvent(1, 0, 0, 3, 0)] = std::bind(&NodeEditor::SHIFT_MMB		, this);
	EventList[GenIntEvent(2, 0, 0, 3, 0)] = std::bind(&NodeEditor::CTRL_MMB			, this);
	EventList[GenIntEvent(0, 0, 0, 0, 1)] = std::bind(&NodeEditor::SCRLL			, this);
	EventList[GenIntEvent(0, 0, 0, 0,-1)] = std::bind(&NodeEditor::SCRLL			, this);
	EventList[GenIntEvent(1, 0, 0, 0, 1)] = std::bind(&NodeEditor::SCRLL			, this);
	EventList[GenIntEvent(1, 0, 0, 0,-1)] = std::bind(&NodeEditor::SCRLL			, this);
	
	Nodes add{ "Add", SCL_MATH_NODE };
	_node_pool.emplace_back(add);

	Nodes sub{ "Subtract", SCL_MATH_NODE };
	sub.SetPos({ 50,50 });
	_node_pool.emplace_back(sub);
}

void NodeEditor::Render(const char* _lable, const ImVec2& _size /*= {0,0}*/)
{
	ImGui::Separator();
	if (ImGui::BeginChild(_lable, ImGui::GetContentRegionAvail())) {
		EventActivate();
		ApplyTransform();

		const ImVec2 pos = ImGui::GetCursorScreenPos();
		NE_size = ImGui::GetContentRegionAvail() + ImVec2(0, 20);
		NE_center = pos + NE_size / 2.0f;

		ImGui::Text(_lable);
		ImGui::GetForegroundDrawList()->AddRectFilled(pos, pos + ImGui::GetContentRegionAvail()+ImVec2(0,20), IM_COL32(255, 255, 255, 20), 5.0f);

		for (auto& node : _node_pool) {
			//RenderNode(node);
			
			const ImVec2 min = NE_center + o_Transform * (node.o_position * glm::vec2(-1, 1) - (ImVec2(50, 40) * node.o_scale / 2.0f));
			const ImVec2 max = NE_center + o_Transform * (node.o_position * glm::vec2(-1, 1) + (ImVec2(50, 40) * node.o_scale / 2.0f));
			
			ImGui::GetWindowDrawList()->AddRectFilled(
				min,
				max,
				IM_COL32(255, 255, 255, 200),
				5.0f
			);
		}


		ImGui::EndChild();
	}
}

void NodeEditor::RenderNode(Nodes& _node)
{

}

void NodeEditor::Resize()
{
	NE_size = ImGui::GetContentRegionAvail() + ImVec2(0, 20);

}

void NodeEditor::SHIFT_MMB()
{
	if (ITEM::is_inside(NE_size))
		Move({ GetDeltaMouseX(), GetDeltaMouseY() });
}

void NodeEditor::CTRL_MMB()
{
	if (ITEM::is_inside(NE_size))
		Zoom(glm::pow(0.8f, -0.05 * dir_float_dist(GetDeltaMouseX(), GetDeltaMouseY())));
}

void NodeEditor::SCRLL()
{
	if (ITEM::is_inside(NE_size))
		Zoom(glm::pow(0.8f, -scroll_dir));
}
