#pragma once
#include "ImguiLayer.h"

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

	enum HoverStatus
	{
		OnViewport = 0,
		OnHandle = 1 << 1,
		OnCameraAxis = 1 << 2,
	};

	HoverStatus viewport_status{ OnViewport };

public:
	Viewport();
	Viewport(const std::string& name);
	Viewport(const std::string& name, GLuint texID);
	Viewport(const std::string& name, GLuint texID, const ImVec2& vp_size);
	~Viewport();
public:

	void RegisterEvents(EventPool& evt) override;
	void RenderLayer(const SceneContext& ctx, const EventPool& evt) override;

private:

	void RenderGrids(const SceneContext& ctx);
	void RenderAxis(const SceneContext& ctx);
	void RenderHandle(const SceneContext& ctx);
};

