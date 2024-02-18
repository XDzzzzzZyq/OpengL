#pragma once

#include "ImguiLayer.h"
#include "RenderConfigs.h"

class RenderConfigViewer : public ImguiLayer
{
public:

	RenderConfigViewer();
	RenderConfigViewer(const std::string& _name);
	~RenderConfigViewer();

public:
	void RenderLayer() override;

	bool RenderOption(char* option, std::string name, const std::vector<std::string>& option_names);
	bool RenderSlider(char);
};

