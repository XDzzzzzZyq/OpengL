#pragma once

#include "ImguiLayer.h"

class MaterialViewer : public ImguiLayer
{
public:

	MaterialViewer();
	MaterialViewer(std::string _name);
	~MaterialViewer();

public:

	void RenderName(std::string& _name, bool read_only = false);

	void RenderLayer() override;
};

