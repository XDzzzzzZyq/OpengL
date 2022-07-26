#pragma once
#include "ImguiLayer.h"
#include "ITEM/TextureView.h"

class Viewport : public ImguiLayer
{
private:

public:
	Viewport();
	Viewport(const std::string& name);
	Viewport(const std::string& name, GLuint texID);
	Viewport(const std::string& name, GLuint texID, const ImVec2& vp_size);
	~Viewport();
public:

	void UpdateLayer() override;
	void RenderLayer() const override;
};

