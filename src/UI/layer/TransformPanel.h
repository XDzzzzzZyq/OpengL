#pragma once

#include "ImguiLayer.h"

class TransformPanel : public ImguiLayer
{

public:

	TransformPanel();
	TransformPanel(std::string _name);
	~TransformPanel();

public:

	void RenderLayer() override;
};

