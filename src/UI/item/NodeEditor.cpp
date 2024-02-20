#include "NodeEditor.h"

#include <cstdint>
#include <string>

#include "operator.h"=

const ImVec2 ImguiNodes::GetInPinPos(const ImVec2& _header_size, float _offset, int _idx)
{
	ImVec2 inp_curs = min + _header_size;
	inp_curs.y += _offset * _idx;
	return inp_curs;
}

ImguiNodes::~ImguiNodes()
{
}

void ImguiNodes::UpdateStates()
{
	int i_off = 0;
	for (int i = 0; auto & i_p : m_node->n_in) {

		if (m_states.find(&i_p) == m_states.end())
			m_states[&i_p] = {};

		const bool is_connected = Nodes::n_in_link.find(&i_p) != Nodes::n_in_link.end();                // is this pin connected
		Parameters* tar_o = is_connected ? Nodes::GetParamPtr(Nodes::n_in_link[&i_p], false) : nullptr; // get the link target ptr
		const Nodes::ParaLink tar_link = is_connected ? Nodes::n_in_link[&i_p] : Nodes::ParaLink{};
		if (!is_connected) i_off++;

		m_states[&i_p] = { tar_o, is_connected, i, tar_link, std::to_string(reinterpret_cast<std::uintptr_t>(&i_p)) + "p", std::to_string(reinterpret_cast<std::uintptr_t>(&i_p)) + "s" };

		i++;
		i_off++;
	}

	int o_off = 0;
	for (int i = 0; auto & o_p : m_node->n_out) {
		if (m_states.find(&o_p) == m_states.end())
			m_states[&o_p] = {};

		const bool is_connected = Nodes::n_out_link.find(&o_p) != Nodes::n_out_link.end();
		Parameters* tar_i = is_connected ? Nodes::GetParamPtr(Nodes::n_out_link[&o_p], true) : nullptr; // get the link target ptr
		const Nodes::ParaLink tar_link = is_connected ? Nodes::n_out_link[&o_p] : Nodes::ParaLink{};

		//m_states[&o_p] = { tar_i, is_connected, i, tar_link, std::to_string((int)&o_p) + "p"};
		m_states[&o_p] = { tar_i, is_connected, i, tar_link, std::to_string(reinterpret_cast<std::uintptr_t>(&o_p)) + "p"};

		i++;
		o_off++;
	}

	max_pin_offset = std::max(i_off, o_off);
}

const ImVec2 ImguiNodes::GetOutPinPos(const ImVec2& _header_size, float _offset, int _idx)
{
	ImVec2 outp_curs = ImVec2(max.x, min.y) + _header_size;
	outp_curs.y += _offset * _idx;
	return outp_curs;
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
	add->PushOut({ VEC4_PARA, "?adwasdw" });
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
	mix->LinkIn(1, add, 1);
	PushNode(mix);

	for (auto& n : _node_pool) n.UpdateStates();
}

void NodeEditor::ResetState()
{
	if (!is_editing_pin_in || is_editing_pin_out)
		editing_in_pin = nullptr;

	if (!is_editing_pin_out || is_editing_pin_in)
		editing_out_pin = nullptr;

	if (!is_editing_pin_in && !is_editing_pin_out)
		pressed_pin = nullptr;

	is_editing_pin_in = is_editing_pin_out = false;
	is_hover_on_in = is_hover_on_out = is_press_on_in = is_press_on_out = false;
	hovered_pin = nullptr;
	editing_cn_type_b = editing_cn_type;
	editing_cn_type = O_I;
	tar_pin_pos = { 0,0 };

	if (!LMB_press)
		is_node_movable = true;
}

float NodeEditor::th_curvity = 1.6f;
float NodeEditor::th_offset = 5;
float NodeEditor::th_rounding = 2;

void NodeEditor::Render(const char* _lable, const ImVec2& _size /*= {0,0}*/)
{
	ImGui::Separator();
	if (ImGui::BeginChild(_lable, ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground)) {


		const ImVec2 pos = ImGui::GetCursorScreenPos();
		NE_size = ImGui::GetContentRegionAvail() + ImVec2(0, 20);
		NE_center = pos + NE_size / 2.0f;


		ImGui::GetForegroundDrawList()->AddRectFilled(pos, pos + ImGui::GetContentRegionAvail(), IM_COL32(255, 255, 255, 20), 5.0f);
		ImGui::Text(_lable);
		ImGui::SliderFloat("B_curvity", &th_curvity, 0, 15);
		ImGui::SliderFloat("N_rounding", &th_rounding, 0, 15);
		ImGui::SliderFloat("P_offset", &th_offset, 2, 10);

#ifdef _DEBUG
		ImGui::Text("in: %i | out: %i | type: %i", is_editing_pin_in, is_editing_pin_out, editing_cn_type);
		ImGui::Text("in_p: 0x%x | out_p: 0x%x", editing_in_pin, editing_out_pin);
		ImGui::Text("act_node: x0%x", (editing_node) ? (*editing_node).m_node : nullptr);
		ImGui::Text("press: 0x%x", pressed_pin);
		ImGui::Text("hover: 0x%x", hovered_pin);
		ImGui::Text("H_in: %i | H_out: %i", is_hover_on_in, is_hover_on_out);
		ImGui::Text("P_in: %i | P_out: %i", is_press_on_in, is_press_on_out);
#endif

		EventActivate();
		ApplyTransform();
		Parameters* pressed_pin_b = pressed_pin;
		Parameters* hovered_pin_b = hovered_pin;
		Parameters* editing_in_pin_b = editing_in_pin;
		Parameters* editing_out_pin_b = editing_out_pin;
		const bool is_hover_on_in_b = is_hover_on_in;
		ResetState();

		ImFont* font = ImGui::GetIO().Fonts->Fonts[1];
		ImGui::PushFont(font);
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.9, 0.9, 0.9, 1);

		///////////////////////// RENDER ///////////////////////////
		const float rounding = th_rounding * o_scale[0];
		const float pin_offset = th_offset * o_scale[0];
		const ImVec2 pin_size = ImVec2(0.1, 0.1) * o_scale;
		const ImVec2 head_size = ImVec2(0, 15) * o_scale;
		const ImVec2 handle_offset = ImVec2(th_curvity * 10, 0) * o_scale;

		const ImVec2 mouse = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();

		const glm::vec2 pos_trans = o_position * o_scale;

		for (auto& node : _node_pool) {
			//RenderNode(node);

			const float pin_list_size = node.max_pin_offset * th_offset;

			node.min = NE_center + o_Transform * (node->o_position * glm::vec2(-1, 1) - (ImVec2(50, 15 + pin_list_size) * node->o_scale / 2.0f));
			node.header = node.min - ImVec2(0, 5) * o_scale;
			node.max = NE_center + o_Transform * (node->o_position * glm::vec2(-1, 1) + (ImVec2(50, 15 + pin_list_size) * node->o_scale / 2.0f));

			const ImDrawFlags flags = node->is_open ? ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight : ImDrawFlags_None;
			// [  HEAD  ]
			ImGui::GetWindowDrawList()->AddRectFilled(
				node.header,
				ImVec2(node.max.x, node.min.y),
				ImguiNodes::n_color_list[node->n_type],
				rounding,
				flags
			);

			const ImVec2 arror_up = node.min - ImVec2(-2.5, 4.0) * o_scale - ImVec2(8, 0.2);
			const ImVec2 arror_dn = node.min - ImVec2(-3.0, 4.2) * o_scale - ImVec2(8, 0.2);
			ImGui::GetWindowDrawList()->AddText(font, 4 * o_scale[0], node.min - ImVec2(-6, 4.5) * o_scale, IM_COL32(255, 255, 255, 255), node->n_name.c_str());
			ImGui::RenderArrow(ImGui::GetWindowDrawList(), node->is_open ? arror_up : arror_dn, IM_COL32(255, 255, 255, 255), node->is_open ? ImGuiDir_Down : ImGuiDir_Right, o_scale[0] * 0.2);

			if (LMB_press && EventListener::IsMouseClick())
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
				///////////////////////////////////////////////////////////////////////////////////////////////
				//////////////////////////////// INPUT pins' event & rendering ////////////////////////////////
				for (auto& i_p : node->n_in) {

					const bool is_connected = node.m_states[&i_p].p_connected;					// is this pin connected
					Parameters* tar_o = node.m_states[&i_p].p_tar;								// get the link target ptr

					bool hovered = false;
					const bool click = ImGui::PinButton(i_p.para_name.c_str(), node.m_states[&i_p].p_ID.c_str(), true, inp_curs, pin_size, false, ImguiNodes::pin_color_list[i_p.para_type], &hovered, is_connected);

#ifdef _DEBUG
					if (hovered) {
						// for debug
						ImGui::Text("this_i: 0x%x", &i_p);
						ImGui::Text(is_connected ? "C" : "NC");
						ImGui::Text("tar_o: 0x%x", tar_o);
					}
#endif


					if (click) {                                  // if on click, update editing event
						pressed_pin = &i_p;
						is_press_on_in = true;
						if (is_connected) {
							is_editing_pin_out |= true;
							editing_out_pin = tar_o;
							editing_cn_type = O_M;
							Nodes::ParaLink& tar_link = node.m_states[&i_p].p_link;
							tar_pin_pos = _node_pool[_node_index_cache[tar_link.first]].GetOutPinPos(head_size, pin_offset, tar_link.second);
							editing_para_type = tar_o->para_type;
						}
						else {
							is_editing_pin_in |= true;
							editing_in_pin = &i_p;
							editing_cn_type = M_I;
							tar_pin_pos = inp_curs;
						}
					}

					if (hovered) {
						hovered_node = &node;
						hovered_pin = &i_p;
						is_hover_on_in = true;
					}

					if (is_connected) {

						const Nodes::ParaLink tar_link = node.m_states[&i_p].p_link;

						ImVec2 start_p;

						if (tar_link.first->is_open)
							start_p = _node_pool[_node_index_cache[tar_link.first]].GetOutPinPos(head_size, pin_offset, tar_link.second);
						else
							start_p = ImVec2(_node_pool[_node_index_cache[tar_link.first]].max.x, _node_pool[_node_index_cache[tar_link.first]].min.y) + ImVec2(margin, margin);

						if (!click && editing_in_pin != &i_p) {
							ImGui::GetWindowDrawList()->AddBezierCubic(                     // static rendering: out -> in
								start_p,
								start_p + ImVec2(th_curvity * 10, 0) * o_scale,
								inp_curs - ImVec2(th_curvity * 10, 0) * o_scale,
								inp_curs,
								ImguiNodes::pin_color_list[i_p.para_type],
								0.5 * o_scale[0]
							);
						}
					}

					//      create connection     |           break connection through output
					if (EventListener::is_mouse_pressed && (editing_in_pin == &i_p)) {
						tar_pin_pos = inp_curs;
						editing_node = &node;
						editing_para_type = i_p.para_type;
					}

					// [ Slider ]
					if (!is_connected) {
						inp_curs.y += pin_offset;
						ImGui::SetCursorScreenPos(inp_curs + ImVec2(o_scale[0] * 2, 0));
						is_node_movable &= !UI::ParaInput::RenderParam(&i_p, node.m_states[&i_p].p_s_ID.c_str(), o_scale[0] / 3, 1.7);
					}

					inp_curs.y += pin_offset;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////
				//////////////////////////////// OUTPUT pins' event & rendering ////////////////////////////////
				ImVec2 outp_curs = ImVec2(node.max.x, node.min.y) + head_size;
				for (auto& o_p : node->n_out) {
					const bool is_connected = node.m_states[&o_p].p_connected;
					Parameters* tar_i = node.m_states[&o_p].p_tar; // get the link target ptr
					Nodes* tar_n = is_connected ? Nodes::n_out_link[&o_p].first : nullptr;

					bool hovered = false;
					const bool click = ImGui::PinButton(o_p.para_name.c_str(), node.m_states[&o_p].p_ID.c_str(), true, outp_curs, pin_size, true, ImguiNodes::pin_color_list[o_p.para_type], &hovered, is_connected);

#ifdef _DEBUG
					if (hovered) {
						ImGui::Text("this_o: 0x%x", &o_p);
						ImGui::Text(is_connected ? "C" : "NC");
						ImGui::Text("tar_i: 0x%x", tar_i);
					}
#endif // _DEBUG

					if (click) {
						pressed_pin = &o_p;
						is_press_on_out = true;
						if (is_connected) {
							editing_in_pin = tar_i;
							is_editing_pin_in |= true;
							editing_cn_type = M_I;
						}
						else {
							editing_out_pin = &o_p;
							is_editing_pin_out |= true;
							editing_cn_type = O_M;
						}
					}

					if (hovered) {
						hovered_node = &node;
						hovered_pin = &o_p;
						is_hover_on_out = true;
					}

					if (EventListener::is_mouse_pressed && editing_out_pin == &o_p) {
						tar_pin_pos = outp_curs;
						editing_node = &node;
						editing_para_type = o_p.para_type;

					}

					outp_curs.y += pin_offset;
				}
			}
			else { // for closed nods
				const float margin = (node.header.y - node.min.y) / 2;
				const ImVec2 pin_pos = node.header - ImVec2(margin, margin);
				for (auto& i : node->n_in) {
					if (Nodes::n_in_link.find(&i) != Nodes::n_in_link.end()) {
						Nodes::ParaLink& tar_link = Nodes::n_in_link[&i];

						ImVec2 end_p;

						if (tar_link.first->is_open)
							end_p = _node_pool[_node_index_cache[tar_link.first]].GetOutPinPos(head_size, pin_offset, tar_link.second);
						else
							end_p = ImVec2(_node_pool[_node_index_cache[tar_link.first]].max.x, _node_pool[_node_index_cache[tar_link.first]].min.y) + ImVec2(margin, margin);

						//DEBUG(_node_pool[_node_index_cache[tar_link.first]]->n_name)
						ImGui::GetWindowDrawList()->AddBezierCubic(
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
			if (node->n_id == active_node_id && is_node_movable && LMB_press)
				if (!is_editing_pin_in && !is_editing_pin_out)
					node->Move((glm::vec2(-1, 1) / o_scale) * EventListener::GetDeltaMouse());

		}

		//[ Active connection ] & [ Break-Connect Opr ]
		switch (editing_cn_type_b)
		{
		case NodeEditor::O_I:

			break;
		case NodeEditor::O_M:
			if (EventListener::IsMouseLeft()) {
				if (hovered_pin_b == nullptr) {							// connect to nothing
					Nodes::BreakLink(pressed_pin_b, Nodes::IN);
					goto skip_O_M;
				}

				if (is_hover_on_out) {									// meaningless
					Nodes::BreakLink(pressed_pin_b, Nodes::OUT);
					goto skip_O_M;
				}

				if (hovered_node != editing_node) {                                                  // connect to other pins
					Nodes::BreakLink(editing_out_pin_b, Nodes::OUT);
					const int self_loc = Nodes::GetParamLoc((*editing_node).m_node, editing_out_pin_b, Nodes::OUT);
					const int tar_loc = Nodes::GetParamLoc((*hovered_node).m_node, hovered_pin_b, Nodes::IN);

					(*hovered_node).m_node->LinkIn(tar_loc, (*editing_node).m_node, self_loc);
					goto skip_O_M;
				}

			skip_O_M:
				if (editing_node)editing_node->UpdateStates();
				if (hovered_node)hovered_node->UpdateStates();
				editing_node = nullptr;
				editing_out_pin = editing_in_pin = nullptr;
				is_editing_pin_in = is_editing_pin_out = false;
			}
			else {
				RenderMark(ADD_MARK, false);
				ImGui::GetWindowDrawList()->AddBezierCubic(
					tar_pin_pos,
					tar_pin_pos + handle_offset,
					ImGui::GetMousePos() - handle_offset,      // handle merge to cursor: out -> cursor
					ImGui::GetMousePos(),
					ImguiNodes::pin_color_list[editing_para_type],
					0.5 * o_scale[0]
				);
			}

			break;
		case NodeEditor::M_I:
			if (EventListener::IsMouseLeft()) {
				if (hovered_pin_b == nullptr) {							//connect to nothing
					Nodes::BreakLink(editing_in_pin_b, Nodes::IN);
					goto skip_M_I;
				}

				if (is_hover_on_in) {
					Nodes::BreakLink(editing_in_pin_b, Nodes::IN);		// meaningless
					goto skip_M_I;
				}


				if (hovered_pin_b != pressed_pin_b) {					// connect to other pins
					Nodes::BreakLink(editing_in_pin_b, Nodes::IN);
					const int self_loc = Nodes::GetParamLoc((*hovered_node).m_node, hovered_pin_b, Nodes::OUT);			//self target location
					const int tar_loc = Nodes::GetParamLoc((*editing_node).m_node, editing_in_pin_b, Nodes::IN);        //get target location

					(*hovered_node).m_node->LinkOut(self_loc, (*editing_node).m_node, tar_loc);
					goto skip_M_I;
				}

			skip_M_I:
				if (editing_node)editing_node->UpdateStates();
				if (hovered_node)hovered_node->UpdateStates();
				editing_node = nullptr;
				editing_out_pin = editing_in_pin = nullptr;
				is_editing_pin_in = is_editing_pin_out = false;
			}
			else {
				RenderMark(ADD_MARK);
				ImGui::GetWindowDrawList()->AddBezierCubic(
					ImGui::GetMousePos(),
					ImGui::GetMousePos() + handle_offset,
					tar_pin_pos - handle_offset,
					tar_pin_pos,
					ImguiNodes::pin_color_list[editing_para_type],
					0.5 * o_scale[0]
				);
			}
			break;
		default:
			break;
		}

	}

	ImGui::PopFont();
	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
	ImGui::EndChild();

	Reset();
}


void NodeEditor::RenderNode(Nodes& _node)
{

}

void NodeEditor::RenderMark(MarkType _type, bool _is_left /*= true*/)
{
	const float offset = 7;
	if (_is_left)
		RenderMark(_type, ImGui::GetMousePos() + ImVec2(-offset, -offset));
	else
		RenderMark(_type, ImGui::GetMousePos() + ImVec2( offset, -offset));
}

void NodeEditor::RenderMark(MarkType _type, ImVec2 _pos)
{
	const float offset_x = 5;
	const float offset_y = 1;
	switch (_type)
	{
	case NodeEditor::NONE_MARK:
		break;
	case NodeEditor::ADD_MARK:
		ImGui::GetWindowDrawList()->AddRectFilled(_pos - ImVec2(offset_x, offset_y), _pos + ImVec2(offset_x, offset_y), IM_COL32(255, 255, 255, 255));
		ImGui::GetWindowDrawList()->AddRectFilled(_pos - ImVec2(offset_y, offset_x), _pos + ImVec2(offset_y, offset_x), IM_COL32(255, 255, 255, 255));
		break;
	case NodeEditor::MINUS_MARK:
		ImGui::GetWindowDrawList()->AddRectFilled(_pos - ImVec2(offset_x, offset_y), _pos + ImVec2(offset_x, offset_y), IM_COL32(255, 255, 255, 255));
		break;
	case NodeEditor::SELECT_MARK:
		break;
	case NodeEditor::MOVE_MARK:
		break;
	case NodeEditor::CUT_MARK:
		break;
	default:
		break;
	}
}

void NodeEditor::Resize()
{
	NE_size = ImGui::GetContentRegionAvail() + ImVec2(0, 20);

}

void NodeEditor::Reset()
{
	if (IsMouseClick()) {
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
	if (Item::is_inside(NE_size))
		Move({ GetDeltaMouseX() / o_scale[0], GetDeltaMouseY() / o_scale[0] });
}

#include "xdz_math.h"
void NodeEditor::CTRL_MMB()
{
	if (Item::is_inside(NE_size))
		Zoom(glm::pow(0.8f, -0.05 * xdzm::dir_float_dist(GetDeltaMouseX(), GetDeltaMouseY())));
}

void NodeEditor::SCRLL()
{
	if (Item::is_inside(NE_size))
		Zoom(glm::pow(0.8f, -scroll_dir));
}


