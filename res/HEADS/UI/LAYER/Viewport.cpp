#include "Viewport.h"
#include "xdz_math.h"
#include "Guizmo/ImGuizmo.h"

Viewport::Viewport()
{
	uly_name = "";
}

Viewport::Viewport(const std::string& name)
	:Viewport(name, 0)
{}

Viewport::Viewport(const std::string& name, GLuint texID)
	: Viewport(name, 0, ImVec2(SCREEN_W, SCREEN_H))
{}

Viewport::Viewport(const std::string& name, GLuint texID, const ImVec2& vp_size)
{
	uly_name = name;
	PushItem<UI::TextureViewer>("Viewport", texID, vp_size);
}

Viewport::~Viewport()
{

}

void Viewport::UpdateLayer()

{
}

void Viewport::RenderLayer()
{
	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		GetLayerSize();
		if (uly_name == "Viewport")ImGui::GetWindowContentRegionMin();
			EventListener::viewport_offset = -(EventListener::window_pos - ImGui::GetWindowPos());
		EventListener::is_in_viewport = ITEM::is_inside(uly_size);

		item_list[0]->RenderItem();

		if (display_grid)
			RenderGrids();
		if (display_axis)
			RenderAxis();
		if (display_trans_handle)
			RenderHandle();

		if (IsResizingFin())
			if (resize_event) {
				item_list[0]->ResetSize(uly_size + ImVec2(10, 10));
				resize_event();
			}

		is_size_changed_b = is_size_changed;
		is_size_changed = false;
		ImGui::End();
	}
	else {
		EventListener::is_in_viewport = false;
		ImGui::End();
	}
}

void Viewport::RenderGrids()
{
	Camera* active_cam = dynamic_cast<Camera*>(EventListener::GetActiveCamera());

	if (active_cam == nullptr)
		return;

	ImGuizmo::DrawGrid(&active_cam->o_InvTransform[0][0], &active_cam->cam_frustum[0][0], &xdzm::identityMatrix[0][0], 30.f, 0.5f);
}

void Viewport::RenderAxis()
{
	Camera* active_cam = dynamic_cast<Camera*>(EventListener::GetActiveCamera());

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;

	ImGuizmo::SetDrawlist();
	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
	viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
	viewManipulateTop = ImGui::GetWindowPos().y;

	if (active_cam == nullptr)
		return;

	glm::mat4 cam_trans = active_cam->o_InvTransform;
	static glm::mat4 test_trans{1};
	ImGuizmo::ViewManipulate(&cam_trans[0][0], 5, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

	active_cam->SetCamTrans(glm::transpose(cam_trans), false, true);
}

void Viewport::RenderHandle()
{

	Transform3D* active_trans = dynamic_cast<Transform3D*>(EventListener::active_object);
	Camera* active_cam = dynamic_cast<Camera*>(EventListener::GetActiveCamera());

	if (active_trans == nullptr)
		return;

	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	//ImGuizmo::DrawCubes(&active_cam->o_InvTransform[0][0], &active_cam->cam_frustum[0][0], &active_trans->o_Transform[0][0], 1);
	glm::mat4 obj_trans = active_trans->o_Transform;

	bool hover, click;
	ImGuizmo::Manipulate(&active_cam->o_InvTransform[0][0], &active_cam->cam_frustum[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &obj_trans[0][0], &hover, &click, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	EventListener::ReportGuizmoStatus(hover, click);

	if(click)
		active_trans->SetTrans(obj_trans);
}
