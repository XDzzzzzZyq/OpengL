#pragma once
#include "ImguiLayer.h"
#include "ITEM/TextureViewer.h"

#include "Guizmo/ImGuizmo.h"

class Viewport : public ImguiLayer
{
public:

	bool display_axis{ true };
	bool display_grid{ true };
	bool display_trans_handle{ true };

	static ImGuizmo::MODE trans_mod;
	static ImGuizmo::OPERATION handle_mod;

public:

	enum ViewPortStatus
	{
		None,
		OnHover,
		OnClick
	};

	bool is_in_viewport;
	ViewPortStatus viewport_status;

public:
	Viewport();
	Viewport(const std::string& name);
	Viewport(const std::string& name, GLuint texID);
	Viewport(const std::string& name, GLuint texID, const ImVec2& vp_size);
	~Viewport();
public:

	void RenderLayer(const SceneContext& ctx, const EventPool& evt) override;

private:

	void RenderGrids(const SceneContext& ctx);
	void RenderAxis(const SceneContext& ctx);
	void RenderHandle(const SceneContext& ctx);

	bool multi_select = false;
	void LMB_CLICK(const SceneContext& ctx);
	void SHIFT(const SceneContext& ctx);

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

