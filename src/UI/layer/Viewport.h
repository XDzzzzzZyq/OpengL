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

	void UpdateLayer(const SceneContext& ctx) override;
	void RenderLayer(const SceneContext& ctx) override;

private:

	void RenderGrids(const SceneContext& ctx);
	void RenderAxis(const SceneContext& ctx);
	void RenderHandle(const SceneContext& ctx);

	bool multi_select = false;
	void EventInit();
	void LMB_CLICK(const SceneContext& ctx);
	void SHIFT(const SceneContext& ctx);

public:

	// Key Input
	static void MTranslate(const SceneContext&);
	static void MRotate(const SceneContext&);
	static void MScale(const SceneContext&);
	static void XAxis(const SceneContext&);
	static void YAxis(const SceneContext&);
	static void ZAxis(const SceneContext&);
	static void WAxis(const SceneContext&);
};

