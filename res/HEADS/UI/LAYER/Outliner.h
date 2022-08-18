#pragma once
#include "ImguiLayer.h"
#include "ImguiTheme.h"

class Outliner : public ImguiLayer, public ImguiTheme
{
private:
	OutlineData ol_data;
	mutable int ol_width = 30;
	int actID;
	int actIndex;
	ImFont* fontA;
public:
	Outliner();
	Outliner(const std::string& name);
	~Outliner();

public:
	void SetObjectList(OutlineData* data) override;
	void SetActiveData(int ID) override;
	void UpdateStyle();

	void RenderLayer() const override;
};

