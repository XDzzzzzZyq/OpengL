#pragma once

#include "ImguiLayer.h"
#include "Transform.h"

class TransformPanel : public ImguiLayer
{

public:

	TransformPanel();
	TransformPanel(std::string _name);
	~TransformPanel();

	static Transform* GetActiveTransPtr();

public:
	void RenderParenting(Transform3D& trans);
	bool RenderTransfroms(Transform3D& trans);

	void RenderLayer() override;
};

