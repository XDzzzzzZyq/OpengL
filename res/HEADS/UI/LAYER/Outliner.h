#pragma once
#include "ImguiLayer.h"
#include "ImguiTheme.h"

class Outliner : public ImguiLayer, public ImguiTheme
{
private:
	mutable OutlineData ol_data;
	mutable std::unordered_map<int, int> id2index;
	mutable std::unordered_map<int, int> index2id;
	mutable int ol_width = 30;
	mutable int actID;
	mutable int actIndex;
	mutable int actIndex_b;
	ImFont* fontA;
public:
	Outliner();
	Outliner(const std::string& name);
	~Outliner();

public:
	void SetObjectList(OutlineData* data)override;
	void UpdateStyle();

	void UpdateLayer() override;
	void RenderLayer() const override;
};

