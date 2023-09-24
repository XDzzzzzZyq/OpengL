#pragma once
#include "ImguiLayer.h"
#include "ITEM/TextureViewer.h"

#include "Guizmo/ImGuizmo.h"
#include "Camera.h"
#include "Transform.h"

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
	void RenderLayer() override;

public: 

	void RenderGrids();
	void RenderGuizmos();
};

