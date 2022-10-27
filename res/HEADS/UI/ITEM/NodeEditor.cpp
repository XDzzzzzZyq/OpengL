#include "NodeEditor.h"

const ImVec2 NodeTrans::GetInPinPos(const ImVec2& _header_size, float _offset, int _idx)
{
	ImVec2 inp_curs = min + _header_size;
	inp_curs.y += _offset * _idx;
	return inp_curs;
}

const ImVec2 NodeTrans::GetOutPinPos(const ImVec2& _header_size, float _offset, int _idx)
{
	ImVec2 outp_curs = ImVec2(max.x, min.y) + _header_size;
	outp_curs.y += _offset * _idx;
	return outp_curs;
}



NodeEditor::NodeEditor()
	:NodeEditor(NONE_NODE_EDITOR)
{
}

NodeEditor::NodeEditor(NodeEditorType type)
	: _type(type)
{
	EventList[GenIntEvent(0, 0, 0, 1, 0)] = std::bind(&NodeEditor::LMB, this);
	EventList[GenIntEvent(0, 0, 0, 3, 0)] = std::bind(&NodeEditor::SHIFT_MMB, this);
	EventList[GenIntEvent(1, 0, 0, 3, 0)] = std::bind(&NodeEditor::SHIFT_MMB, this);
	EventList[GenIntEvent(2, 0, 0, 3, 0)] = std::bind(&NodeEditor::CTRL_MMB, this);
	EventList[GenIntEvent(0, 0, 0, 0, 1)] = std::bind(&NodeEditor::SCRLL, this);
	EventList[GenIntEvent(0, 0, 0, 0, -1)] = std::bind(&NodeEditor::SCRLL, this);
	EventList[GenIntEvent(1, 0, 0, 0, 1)] = std::bind(&NodeEditor::SCRLL, this);
	EventList[GenIntEvent(1, 0, 0, 0, -1)] = std::bind(&NodeEditor::SCRLL, this);

	Zoom(3.0f);

	Nodes* add = new Nodes{ "Add", SCL_MATH_NODE };
	add->PushIn({ INT_PARA, "int1" });
	add->PushIn({ INT_PARA, "int2" });
	add->PushIn({ VEC2_PARA, "vec2_test" });
	add->PushIn({ VEC3_PARA, "vec3_test" });
	add->PushOut({ INT_PARA, "int3" });
	add->PushOut({ VEC4_PARA, "vec4" });
	PushNode(add);

	Nodes* sub = new Nodes{ "Subtract", VEC_MATH_NODE };
	sub->SetPos({ 100,50 });
	sub->PushOut({ FLOAT_PARA, "outp" });
	sub->PushOut({ VEC2_PARA, "outp2" });
	add->LinkIn(0, sub, 0);
	add->LinkIn(2, sub, 1);
	PushNode(sub);

	Nodes* sub2 = new Nodes{ "Subtract2", VEC_MATH_NODE };
	sub2->SetPos({ 100,-50 });
	sub2->PushOut({ VEC3_PARA, "outp" });
	sub2->PushOut({ VEC4_PARA, "outp2" });
	add->LinkIn(3, sub2, 0);
	PushNode(sub2);

	Nodes* mix = new Nodes{ "Mixture", VEC_MATH_NODE };
	mix->SetPos({ -100, 0 });
	mix->PushIn({ VEC4_PARA, "inp" });
	mix->PushIn({ VEC4_PARA, "inp2" });
	mix->PushIn({ VEC2_PARA, "inp3" });
	mix->LinkIn(0, sub2, 1);
	mix->LinkIn(1, add,  1);
	PushNode(mix);
}

void NodeEditor::Render(const char* _lable, const ImVec2& _size /*= {0,0}*/)
{
	ImGui::Separator();
	if (ImGui::BeginChild(_lable, ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground)) {
		EventActivate();
		ApplyTransform();

		const ImVec2 pos = ImGui::GetCursorScreenPos();
		NE_size = ImGui::GetContentRegionAvail() + ImVec2(0, 20);
		NE_center = pos + NE_size / 2.0f;


		ImGui::GetForegroundDrawList()->AddRectFilled(pos, pos + ImGui::GetContentRegionAvail(), IM_COL32(255, 255, 255, 20), 5.0f);
		ImGui::Text(_lable);
		ImGui::SliderFloat("B_curvity", &th_curvity, 0, 15);
		ImGui::SliderFloat("N_rounding", &th_rounding, 0, 15);
		ImGui::SliderFloat("P_offset", &th_offset, 2, 10);

		ImFont* font = ImGui::GetIO().Fonts->Fonts[1];
		ImGui::PushFont(font);
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.9, 0.9, 0.9, 1);

		///////////////////////// RENDER ///////////////////////////
		const float rounding = th_rounding * o_scale[0];

		const float pin_offset = th_offset * o_scale[0];
		const ImVec2 pin_size = ImVec2(0.1, 0.1) * o_scale;
		const ImVec2 head_size = ImVec2(0, 15) * o_scale;

		const ImVec2 mouse = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();

		const glm::vec2 pos_trans = o_position * o_scale;

		for (auto& node : _node_pool) {
			//RenderNode(node);

			node.min = NE_center + o_Transform * (node->o_position * glm::vec2(-1, 1) - (ImVec2(50, 40) * node->o_scale / 2.0f));
			node.header = node.min - ImVec2(0, 5) * o_scale;
			node.max = NE_center + o_Transform * (node->o_position * glm::vec2(-1, 1) + (ImVec2(50, 40) * node->o_scale / 2.0f));

			const ImDrawFlags flags = node->is_open ? ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight : ImDrawFlags_None;
			// [  HEAD  ]
			ImGui::GetWindowDrawList()->AddRectFilled(
				node.header,
				ImVec2(node.max.x, node.min.y),
				Nodes::n_color_list[node->n_type],
				rounding,
				flags
			);

			const ImVec2 arror_up = node.min - ImVec2(-2.5, 4.0) * o_scale - ImVec2(8, 0.2);
			const ImVec2 arror_dn = node.min - ImVec2(-3.0, 4.2) * o_scale - ImVec2(8, 0.2);
			ImGui::GetWindowDrawList()->AddText(font, 4 * o_scale[0], node.min - ImVec2(-6, 4.5) * o_scale, IM_COL32(255, 255, 255, 255), node->n_name.c_str());
			ImGui::RenderArrow(ImGui::GetWindowDrawList(), node->is_open ? arror_up : arror_dn, IM_COL32(255, 255, 255, 255), node->is_open ? ImGuiDir_Down : ImGuiDir_Right, o_scale[0] * 0.2);

			if (IsClick())
				if (node.header < ImGui::GetMousePos() && ImGui::GetMousePos() < node.max) {
					if (ImGui::GetMousePos() < ImVec2(node.header.x + (node.max.x - node.header.x) * 0.125, node.min.y))
						node->is_open = !node->is_open;
					else
						active_node_id = node->n_id;

					no_node_clicked &= false;
				}

			//[  Outline  ]
			if (node->n_id == active_node_id)
				ImGui::GetWindowDrawList()->AddRect(
					node.header,
					node->is_open ? node.max : ImVec2(node.max.x, node.min.y),
					IM_COL32(255, 255, 255, 255),
					rounding,
					0,
					2.0f
				);

			bool editing_link = false;
			// [  BODY  ]
			if (node->is_open) {
				ImGui::GetWindowDrawList()->AddRectFilled(
					node.min,
					node.max,
					IM_COL32(10, 10, 10, 255),
					rounding,
					ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight
				);

				ImVec2 inp_curs = node.min + head_size;
				const float margin = (node.header.y - node.min.y) / 2;
				for (auto& i_p : node->n_in) {
					const bool is_connected = Nodes::n_in_link.find(&i_p) != Nodes::n_in_link.end();

					editing_link |= ImGui::PinButton(i_p.para_name.c_str(), true, inp_curs, pin_size, false, Nodes::pin_color_list[i_p.para_type], is_connected);

					if (is_connected) {
						Nodes::ParaLink& tar_link = Nodes::n_in_link[&i_p];

						ImVec2 start_p;

						if (tar_link.first->is_open)
							start_p = _node_pool[_node_index_cache[tar_link.first]].GetOutPinPos(head_size, pin_offset, tar_link.second);
						else
							start_p = ImVec2(_node_pool[_node_index_cache[tar_link.first]].max.x, _node_pool[_node_index_cache[tar_link.first]].min.y) + ImVec2(margin, margin);
						
						ImGui::GetWindowDrawList()->AddBezierCurve(
							start_p,
							start_p + ImVec2(th_curvity * 10, 0) * o_scale,
							inp_curs - ImVec2(th_curvity * 10, 0) * o_scale,
							inp_curs,
							Nodes::pin_color_list[i_p.para_type],
							0.5 * o_scale[0]
						);

					}

					inp_curs.y += pin_offset;
				}

				ImVec2 outp_curs = ImVec2(node.max.x, node.min.y) + head_size;
				for (auto& o_p : node->n_out) {
					const bool is_connected = Nodes::n_out_link.find(&o_p) != Nodes::n_out_link.end();

					editing_link |= ImGui::PinButton(o_p.para_name.c_str(), true, outp_curs, pin_size, true, Nodes::pin_color_list[o_p.para_type], is_connected);
					outp_curs.y += pin_offset;
				}
			}
			else {
				const float margin = (node.header.y - node.min.y) / 2;
				const ImVec2 pin_pos = node.header - ImVec2(margin, margin);
				for (auto& i : node->n_in) {
					if (Nodes::n_in_link.find(&i) != Nodes::n_in_link.end()) {
						Nodes::ParaLink& tar_link = Nodes::n_in_link[&i];

						ImVec2 end_p;
						
						if(tar_link.first->is_open)
							end_p = _node_pool[_node_index_cache[tar_link.first]].GetOutPinPos(head_size, pin_offset, tar_link.second);
						else
							end_p = ImVec2(_node_pool[_node_index_cache[tar_link.first]].max.x, _node_pool[_node_index_cache[tar_link.first]].min.y) + ImVec2(margin, margin);

						//DEBUG(_node_pool[_node_index_cache[tar_link.first]]->n_name)
						ImGui::GetWindowDrawList()->AddBezierCurve(
							end_p,
							end_p + ImVec2(th_curvity * 10, 0) * o_scale,
							pin_pos - ImVec2(th_curvity * 10, 0) * o_scale,
							pin_pos,
							IM_COL32(255, 255, 255, 255),
							0.5 * o_scale[0]
						);
					}
				}
			}

			//[  Move  ]
			if (node->n_id == active_node_id && LMB_press)
				if (!editing_link)
					node->Move((glm::vec2(-1, 1) / o_scale) * EventListener::GetDeltaMouse());

		}

		ImGui::PopFont();
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
		ImGui::EndChild();

		Reset();
	}

}

void NodeEditor::RenderNode(Nodes& _node)
{

}

void NodeEditor::Resize()
{
	NE_size = ImGui::GetContentRegionAvail() + ImVec2(0, 20);

}

void NodeEditor::Reset()
{
	if (IsClick()) {
		if (no_node_clicked)
			active_node_id = 0;

		no_node_clicked = true;
	}

	LMB_press = false;
}

void NodeEditor::LMB()
{
	LMB_press = true;
}

void NodeEditor::SHIFT_MMB()
{
	if (ITEM::is_inside(NE_size))
		Move({ GetDeltaMouseX() / o_scale[0], GetDeltaMouseY() / o_scale[0] });
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


