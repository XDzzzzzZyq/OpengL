#pragma once
#include "ImguiLayer.h"

class ParamControl : public ImguiLayer
{
public:
	ParamControl();
	ParamControl(const std::string& name);
	~ParamControl();

public:
	void RenderLayer() override;
};

