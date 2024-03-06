#pragma once
#include "Nodes.h"

#include "ImguiItem.h"
#include "ParaInput.h"
#include "ImGui/imgui_internal.h"
#include "EventListener.h"

enum NodeEditorType
{
	NONE_NODE_EDITOR = -1, SHADER_NODE_EDITOR
};

struct PinStates
{
	Parameters* p_tar = nullptr;
	bool p_connected = false;
	int p_offset = 0;
	Nodes::ParaLink p_link = {};
	std::string p_ID = "";
	std::string p_s_ID = "";
};

struct ImguiNodes {

public:
	ImguiNodes(Nodes* _node) :m_node(_node) {  }
	~ImguiNodes();
	Nodes* m_node;

public:
	std::unordered_map<Parameters*, PinStates> m_states;
	void UpdateStates();

public:
	ImVec2 min{ 0,0 };
	ImVec2 max{ 0,0 };
	ImVec2 header{ 0,0 };
	int max_pin_offset = 0;

public:
	const ImVec2 GetOutPinPos(const ImVec2& _header_size, float _offset, int _idx);
	const ImVec2 GetInPinPos(const ImVec2& _header_size, float _offset, int _idx);
	
	Nodes* GetNodePtr() const { return m_node; }
public:
	Nodes* operator->() { return m_node; }
public:
	bool is_in_editing{ false };
	bool is_out_editing{ false };

public:

	static std::vector<ImU32> n_color_list;
	static std::vector<ImU32> pin_color_list;

};

class NodeEditor : public EventListener, public Transform2D
{
private:
	enum ConnectType { O_I, O_M, M_I };
	enum MarkType { NONE_MARK, ADD_MARK, MINUS_MARK, SELECT_MARK, MOVE_MARK, CUT_MARK };
private:
	std::vector<ImguiNodes> _node_pool;
	std::unordered_map<Nodes*, int> _node_index_cache;
	void PushNode(Nodes* _node) { _node_pool.emplace_back(_node); _node_index_cache[_node] = (int)_node_pool.size() - 1; }
	void PushNodes(const std::vector<Nodes*>& _list) {
		for (auto& node : _list) { 
			_node_pool.emplace_back(node);
			_node_index_cache[node] = (int)_node_pool.size() - 1; 
		}
	}

	ConnectType editing_cn_type = O_I;
	ConnectType editing_cn_type_b = O_I;
	ParaType editing_para_type{};
	unsigned int active_node_id{};
	Parameters* editing_in_pin{};
	Parameters* editing_out_pin{};
	Parameters* hovered_pin{};
	Parameters* pressed_pin{};
	ImguiNodes* editing_node{};
	ImguiNodes* hovered_node{};

	ImVec2 tar_pin_pos{};
private:
	bool no_node_clicked = true;
	bool is_editing_pin_in{};
	bool is_editing_pin_out{};
	bool is_hover_on_in{};
	bool is_press_on_in{};
	bool is_hover_on_out{};
	bool is_press_on_out{};
	bool LMB_press = false;

	bool is_node_movable{};

	void ResetState();
private:
	static float th_curvity;
	static float th_offset;
	static float th_rounding;
public:
	NodeEditorType _type{NONE_NODE_EDITOR};
	NodeEditor(NodeEditorType type);
	NodeEditor() {};

public:
	void Render(const char* _lable, const ImVec2& _size = {0,0});

	static void RenderNode(Nodes& _node);
	static void RenderMark(MarkType _type, bool _is_left = true);
	static void RenderMark(MarkType _type, ImVec2 _pos);
public:
	ImVec2 NE_size, NE_center;
	void Resize();
	void Reset();

	void LMB();
	void SHIFT_MMB();
	void CTRL_MMB();
	void SCRLL();
};

