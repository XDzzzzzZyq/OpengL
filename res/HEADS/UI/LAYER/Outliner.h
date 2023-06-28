#pragma once
#include "ImguiLayer.h"
#include "ImguiTheme.h"

class Outliner : public ImguiLayer, public ImguiTheme
{
private:
	OutlineData ol_data;
	std::unordered_map<int, int> id2index;
	std::unordered_map<int, int> index2id;
	int ol_width = 30;
	int actID;
	int actIndex;
	int actIndex_b;
	ImFont* fontA;
public:
	Outliner();
	Outliner(const std::string& name);
	~Outliner();

public:
	void SetObjectList(OutlineData* data)override;
	void UpdateStyle();

	void UpdateLayer() override;
	void RenderLayer() override;
};

