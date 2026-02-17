#pragma once

#include "ImguiLayer.h"

class MaterialViewer : public ImguiLayer
{
public:

	MaterialViewer();
	MaterialViewer(std::string _name);
	~MaterialViewer();

public:

	bool RenderName(const std::string& name, std::string& out_name, bool read_only = false);

	void RenderLayer(const Context& ctx, const EventPool& evt) override;
};

