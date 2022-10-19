#pragma once

#include "support.h"
#include "Nodes.h"

#include "ImguiItem.h"
#include "EventListener.h"

enum NodeEditorType
{
	NONE_NODE_EDITOR = -1, SHADER_NODE_EDITOR
};

class NodeEditor : public EventListener, public Transform2D
{
private:
	std::vector<Nodes> _node_pool;
public:
	NodeEditorType _type{NONE_NODE_EDITOR};
	NodeEditor(NodeEditorType type);
	NodeEditor();

public:
	void Render(const char* _lable, const ImVec2& _size = {0,0});

	static void RenderNode(Nodes& _node);
public:
	ImVec2 NE_size, NE_center;
	void Resize();

	void SHIFT_MMB();
	void CTRL_MMB();
	void SCRLL();
};

