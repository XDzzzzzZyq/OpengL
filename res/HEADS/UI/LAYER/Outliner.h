#pragma once
#include "ImguiLayer.h"
#include "ImguiTheme.h"

class Outliner : public ImguiLayer, public ImguiTheme
{
private:
	OutlineData ol_data;
	std::unordered_map<int, int> id2index; // id -> the index of element
	std::unordered_map<int, int> index2id; // index -> id of the current object
	int ol_width = 30;
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

