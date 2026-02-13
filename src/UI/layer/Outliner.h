#pragma once
#include "ImguiLayer.h"
#include "ImguiTheme.h"

class Outliner : public ImguiLayer, public ImguiTheme
{
private:

	int ol_width = 30;
	ImFont* fontA;
public:
	Outliner();
	Outliner(const std::string& name);
	~Outliner();

public:
	void UpdateStyle();

	void RenderLayer(const SceneContext& ctx, const EventPool& evt) override;
};

