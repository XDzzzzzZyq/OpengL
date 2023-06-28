#pragma once

#include "ImguiLayer.h"
#include "Material.h"

class MaterialViewer : public ImguiLayer
{
public:

	MaterialViewer();
	MaterialViewer(std::string _name);
	~MaterialViewer();

	static Material* GetActiveMatPtr();

public:

	void RenderName(std::string& _name, bool read_only = false);
	bool RenderMatParam(MatParaType _type, Material::MatParamData& _param);

	void RenderLayer() override;
};

