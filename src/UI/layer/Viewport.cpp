#include "Viewport.h"
#include "Input.h"

#include "Camera.h"
#include "Transform.h"
#include "SceneManager.h"
#include "buffer/FrameBuffer.h"

#include "events/CameraEvents.h"

#include "xdz_matrix.h"

ImGuizmo::MODE Viewport::trans_mod = ImGuizmo::WORLD;
ImGuizmo::OPERATION Viewport::handle_mod = ImGuizmo::TRANSLATE;

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


int GetSelectID(const FrameBuffer* info_fb, GLuint x, GLuint y)
{
	if (-glm::vec2(5, 5) < glm::vec2(x, y) && glm::vec2(x, y) < info_fb->GetFrameBufferSize() * glm::vec2(1, 2))
		//return GetActiveEnvironment()->envir_frameBuffer->ReadPix(x - viewport_offset.x, y - viewport_offset.y, ID_FB).GetID();
		return info_fb->ReadPix(x, y, ID_FB).GetID();
	else
		return -1;
}

void Viewport::LMB_CLICK(const SceneContext& ctx)
{
	if (!Input::IsMouseClicked()) return;
	if (!is_in_viewport) return;
	if (viewport_status != ViewPortStatus::None) return;

	const ObjectID* selected_obj = ctx.c_selections.GetSelectedObjects();

	const FrameBuffer* info_fb = (FrameBuffer*)(ctx.c_active_fb_channel);
	SceneResource* scene = dynamic_cast<SceneResource*>(ctx.c_active_scene);

	glm::vec2 offset = VecConvert<ImVec2, glm::vec2>(ImGui::GetWindowPos() - ImGui::GetMainViewport()->Pos);
	int id = GetSelectID(info_fb, GLuint(Input::GetMousePosX() - offset.x), GLuint(Input::GetMousePosY() - offset.y));
	if (scene->GetObjectID(id) == selected_obj) return;

	// TODO: event system
	ctx.c_selections.Select(scene->GetObjectID(id), multi_select);
	Input::input_state.viewport.is_selected_changed = true;
}

void Viewport::SHIFT(const SceneContext& ctx)
{
	multi_select = true;
}

void Viewport::RenderLayer(const SceneContext& ctx, const EventPool& evt)
{
	if (ImGui::Begin(uly_name.c_str(), &uly_is_rendered)) {

		GetLayerSize();
		if (uly_name == "Viewport"){
			viewport_status = ViewPortStatus::None;
			const ImVec2 mouse_pos = ImVec2(Input::GetMousePosX(), Input::GetMousePosY());
			is_in_viewport = Item::is_inside(uly_size, mouse_pos);
		}

		item_list[0]->RenderItem();

#if _DEBUG
		ImGui::Text("[ %d ]", is_in_viewport);
		ImGui::Text("[ %.0f , %.0f ]", ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		ImGui::Text("[ %.0f , %.0f ]", ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowContentRegionMin().y);
		ImGui::Text("[ %.0f , %.0f ]", ImGui::GetMainViewport()->Pos.x, ImGui::GetMainViewport()->Pos.y);
		ImGui::Text("[ %.0f , %.0f ]", ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
		ImGui::Text("[ %.0f , %.0f ]", Input::GetMousePosX(), Input::GetMousePosY());
		ImVec2 window_pos = (ImGui::GetWindowPos() - ImGui::GetMainViewport()->Pos);
		ImGui::Text("[ %.0f , %.0f ]", Input::GetMousePosX() - window_pos.x, Input::GetMousePosY() - window_pos.y);
#endif // _DEBUG

		if (display_grid)
			RenderGrids(ctx);
		if (display_axis)
			RenderAxis(ctx);
		if (display_trans_handle)
			RenderHandle(ctx);

		if (IsResizingFin())
			if (resize_event) {
				item_list[0]->ResetSize(uly_size + ImVec2(10, 10));
				resize_event();
			}

		is_size_changed_b = is_size_changed;
		is_size_changed = false;

		if (is_in_viewport) {
			if (Input::IsMousePressed() && Input::input_state.mouse.button == Input::MouseButtons::MMB) {
				const Camera* active_cam = dynamic_cast<const Camera*>(ctx.GetActiveCamera());
				if (Input::input_state.key.FirstKey == Input::SpecialKeys::CTRL) {
					evt.emit(CameraPushEvent{ (Camera*)active_cam, Input::GetDeltaMouseX(), Input::GetDeltaMouseY()});
				}
				else if (Input::input_state.key.FirstKey == Input::SpecialKeys::SHIFT) {
					evt.emit(CameraSlideEvent{ (Camera*)active_cam, Input::GetDeltaMouseX(), Input::GetDeltaMouseY() });
				}
				else if (Input::input_state.key.FirstKey == Input::SpecialKeys::ALT) {
					evt.emit(CameraSpinEvent{ (Camera*)active_cam, Input::GetDeltaMouseX(), Input::GetDeltaMouseY() });
				}
				else if (Input::input_state.key.FirstKey == Input::SpecialKeys::NONE) {
					evt.emit(CameraRotateEvent{ (Camera*)active_cam, Input::GetDeltaMouseX(), Input::GetDeltaMouseY() });
				}
			}

			if (Input::IsMouseScrolled()) {
				const Camera* active_cam = dynamic_cast<const Camera*>(ctx.GetActiveCamera());
				evt.emit(CameraZoomEvent{ (Camera*)active_cam, Input::GetScrollY() });
			}
		}

		ImGui::End();
	}
	else {
		is_in_viewport = false;
		ImGui::End();
	}
}

void Viewport::RenderGrids(const SceneContext& ctx)
{
	const Camera* active_cam = dynamic_cast<const Camera*>(ctx.GetActiveCamera());

	if (active_cam == nullptr)
		return;

	ImGuizmo::DrawGrid(&active_cam->o_InvTransform[0][0], &active_cam->cam_frustum[0][0], &xdzm::identityMatrix[0][0], 30.f, 0.5f);
}

void Viewport::RenderAxis(const SceneContext& ctx)
{
	Camera* active_cam = (Camera*)dynamic_cast<const Camera*>(ctx.GetActiveCamera());

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

	// TODO: event system
	active_cam->SetCamTrans(glm::transpose(cam_trans), false, true);
}

void Viewport::RenderHandle(const SceneContext& ctx)
{
	SceneResource* scene = dynamic_cast<SceneResource*>(ctx.c_active_scene);
	Camera* active_cam = dynamic_cast<Camera*>(scene->GetActiveCamera());

	SelectionManager<ObjectID>& sel = ctx.c_selections;
	Transform3D* active_trans = dynamic_cast<Transform3D*>(sel.GetActiveObject());

	if (active_trans == nullptr || active_cam == nullptr)
		return;

	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	//ImGuizmo::DrawCubes(&active_cam->o_InvTransform[0][0], &active_cam->cam_frustum[0][0], &active_trans->o_Transform[0][0], 1);
	glm::mat4 obj_trans = active_trans->o_Transform;

	bool hover, click;
	ImGuizmo::Manipulate(&active_cam->o_InvTransform[0][0], &active_cam->cam_frustum[0][0], Viewport::handle_mod, Viewport::trans_mod, &obj_trans[0][0], &hover, &click, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	
	if (hover)
		viewport_status = ViewPortStatus::OnHover;
	if (click)
		viewport_status = ViewPortStatus::OnClick;

	if(click)
		active_trans->SetTrans(obj_trans);
}

void Viewport::MTranslate()
{
	Viewport::handle_mod = ImGuizmo::TRANSLATE;
}

void Viewport::MRotate()
{
	Viewport::handle_mod = ImGuizmo::ROTATE;
}

void Viewport::MScale()
{
	Viewport::handle_mod = ImGuizmo::SCALE;
}

void _SwitchHMode(GLuint offset) {

	if (offset > 2 && Viewport::handle_mod & ImGuizmo::TRANSLATE)
		return;

	ImGuizmo::OPERATION trans = ImGuizmo::OPERATION(ImGuizmo::TRANSLATE_X << offset);
	ImGuizmo::OPERATION rotat = ImGuizmo::OPERATION(ImGuizmo::ROTATE_X << offset);
	ImGuizmo::OPERATION scale = ImGuizmo::OPERATION(ImGuizmo::SCALE_X << offset);

	if (Viewport::handle_mod & ImGuizmo::TRANSLATE) {
		if (Viewport::handle_mod == trans)
			Viewport::MTranslate();
		else
			Viewport::handle_mod = trans;
	}
	else if (Viewport::handle_mod & ImGuizmo::ROTATE) {
		if (Viewport::handle_mod == rotat)
			Viewport::MRotate();
		else
			Viewport::handle_mod = rotat;
	}
	else if (Viewport::handle_mod & ImGuizmo::SCALE) {
		if (Viewport::handle_mod == scale)
			Viewport::MScale();
		else
			Viewport::handle_mod = scale;
	}
}

void Viewport::XAxis()
{
	if (!Input::IsKeyClicked())
		return;

	::_SwitchHMode(0);
}

void Viewport::YAxis()
{
	if (!Input::IsKeyClicked())
		return;

	::_SwitchHMode(1);
}

void Viewport::ZAxis()
{
	if (!Input::IsKeyClicked())
		return;

	::_SwitchHMode(2);
}

void Viewport::WAxis()
{
	if (!Input::IsKeyClicked())
		return;

	::_SwitchHMode(3);
}
