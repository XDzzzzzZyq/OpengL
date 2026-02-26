#pragma once

#include "ImguiLayer.h"

class MaterialViewer : public ImguiLayer
{
public:

	MaterialViewer();
	MaterialViewer(std::string _name);
	~MaterialViewer();

public:

	void RenderLayer(const Context& ctx, EventPool& evt) override;
};

