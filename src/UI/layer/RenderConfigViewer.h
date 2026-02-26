#pragma once

#include "ImguiLayer.h"

class RenderConfigViewer : public ImguiLayer
{
public:

	RenderConfigViewer();
	RenderConfigViewer(const std::string& _name);
	~RenderConfigViewer();

public:
	void RenderLayer(const Context& ctx, EventPool& evt) override;

	bool RenderOption(char* option, std::string name, const std::vector<std::string>& option_names);
	bool RenderSlider(char);
};

