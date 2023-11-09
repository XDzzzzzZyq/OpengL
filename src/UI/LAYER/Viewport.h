#pragma once
#include "ImguiLayer.h"
#include "ITEM/TextureViewer.h"

#include "Guizmo/ImGuizmo.h"

#include "Camera.h"
#include "Transform.h"

class Viewport : public ImguiLayer
{
public:

	bool display_axis{ true };
	bool display_grid{ true };
	bool display_trans_handle{ true };

	static ImGuizmo::MODE trans_mod;
	static ImGuizmo::OPERATION handle_mod;

public:
	Viewport();
	Viewport(const std::string& name);
	Viewport(const std::string& name, GLuint texID);
	Viewport(const std::string& name, GLuint texID, const ImVec2& vp_size);
	~Viewport();
public:

	void UpdateLayer() override;
	void RenderLayer() override;

private:

	void RenderGrids();
	void RenderAxis();
	void RenderHandle();

public:

	// Key Input
	static void MTranslate();
	static void MRotate();
	static void MScale();
	static void XAxis();
	static void YAxis();
	static void ZAxis();
	static void WAxis();
};

