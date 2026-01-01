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

	void UpdateLayer(const SceneContext& ctx) override;
	void RenderLayer(const SceneContext& ctx) override;
};

