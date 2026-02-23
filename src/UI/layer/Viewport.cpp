#include "Viewport.h"
#include "Input.h"

#include "ITEM/TextureViewer.h"

#include "Camera.h"
#include "Transform.h"
#include "SceneManager.h"
#include "buffer/FrameBuffer.h"

#include "events/CameraEvents.h"
#include "events/KeyMouseEvents.h"
#include "events/ViewportEvents.h"

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


void MTranslate()
{
	Viewport::handle_mod = ImGuizmo::TRANSLATE;
}

void MRotate()
{
	Viewport::handle_mod = ImGuizmo::ROTATE;
}

void MScale()
{
	Viewport::handle_mod = ImGuizmo::SCALE;
}

void _SpecifyAxis(GLuint offset) {
	// offset: 0, 1, 2, 3 -> X, Y, Z, W

	if (Viewport::handle_mod & ImGuizmo::TRANSLATE) {
		const ImGuizmo::OPERATION trans = ImGuizmo::OPERATION(ImGuizmo::TRANSLATE_X << offset);
		if (Viewport::handle_mod == trans)
			::MTranslate();			// Reset to full translate
		else
			Viewport::handle_mod = trans;	// Or specify axis
	}
	else if (Viewport::handle_mod & ImGuizmo::ROTATE) {
		const ImGuizmo::OPERATION rotate = ImGuizmo::OPERATION(ImGuizmo::ROTATE_X << offset);
		if (Viewport::handle_mod == rotate)
			::MRotate();
		else
			Viewport::handle_mod = rotate;
	}
	else if (Viewport::handle_mod & ImGuizmo::SCALE) {
		const ImGuizmo::OPERATION scale = ImGuizmo::OPERATION(ImGuizmo::SCALE_X << offset);
		if (Viewport::handle_mod == scale)
			::MScale();
		else
			Viewport::handle_mod = scale;
	}
}

void XAxis()
{
	::_SpecifyAxis(0);
}

void YAxis()
{
	::_SpecifyAxis(1);
}

void ZAxis()
{
	::_SpecifyAxis(2);
}

void WAxis()
{
	if (Viewport::handle_mod & (ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::SCALE))
		return; // W axis is only for rotation

	::_SpecifyAxis(3);
}

void Viewport::RegisterEvents(EventPool& evt)
{
	evt.subscribe<KeyClickEvent>([this](KeyClickEvent e) {
		if (!is_mouse_hovered) return;

		switch (e.key.normal) {
		case Input::NormalKeyFromChar('G'): // G
			::MTranslate();	break;
		case Input::NormalKeyFromChar('R'): // R
			::MRotate(); break;
		case Input::NormalKeyFromChar('S'): // S
			::MScale();	break;
		case Input::NormalKeyFromChar('X'): // X
			::XAxis(); break;
		case Input::NormalKeyFromChar('Y'): // Y
			::YAxis(); break;
		case Input::NormalKeyFromChar('Z'): // Z
			::ZAxis(); break;
		case Input::NormalKeyFromChar('W'): // W
			::WAxis(); break;
		}
		});

	evt.subscribe<MouseClickEvent>([this, &evt](MouseClickEvent e) {
		if (!is_mouse_hovered) return;
		if (viewport_status != HoverStatus::OnViewport) return;

		if (e.mouse == Input::MouseButtons::LMB) {
			int mouse_x = int(Input::GetMousePosX() - uly_pos.x);
			int mouse_y = int(Input::GetMousePosY() - uly_pos.y);
			evt.emit(ViewportSelectedEvent{ mouse_x, mouse_y, e.key == Input::SHIFT });
		}
		});

	evt.subscribe<ViewportImageResetEvent>([this](ViewportImageResetEvent e) {
		ImguiItem* item = FindImguiItem(0);
		if (item != nullptr) {
			item->ResetBufferID(e.tex_id);
		}
		});
}

void Viewport::RenderLayer(const Context& ctx, const EventPool& evt)
{
	item_list[0]->RenderItem();

#if _DEBUG
	ImGui::Text("[ %d ]", is_mouse_hovered);
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

	if (IsResized()) {
		item_list[0]->ResetSize(uly_size + ImVec2(10, 10));
		evt.emit(ViewportResizeEvent{ int(uly_size.x), int(uly_size.y) });
	}

	if (is_mouse_hovered) {
		if (Input::IsMousePressed(Input::MouseButtons::MMB)) {
			const Camera* active_cam = dynamic_cast<const Camera*>(ctx.scene.GetActiveCamera());
			if (Input::IsKeyPressed(Input::CTRL)) {
				evt.emit(CameraPushEvent{ (Camera*)active_cam, Input::GetDeltaMouseX(), Input::GetDeltaMouseY()});
			}
			else if (Input::IsKeyPressed(Input::SHIFT)) {
				evt.emit(CameraSlideEvent{ (Camera*)active_cam, Input::GetDeltaMouseX(), Input::GetDeltaMouseY() });
			}
			else if (Input::IsKeyPressed(Input::ALT)) {
				evt.emit(CameraSpinEvent{ (Camera*)active_cam, Input::GetDeltaMouseX(), Input::GetDeltaMouseY() });
			}
			else if (Input::IsKeyPressed(Input::NONE)) {
				evt.emit(CameraRotateEvent{ (Camera*)active_cam, Input::GetDeltaMouseX(), Input::GetDeltaMouseY() });
			}
		}

		if (Input::IsMouseScrolled()) {
			const Camera* active_cam = dynamic_cast<const Camera*>(ctx.scene.GetActiveCamera());
			evt.emit(CameraZoomEvent{ (Camera*)active_cam, Input::GetScrollY() });
		}
	}
}

void Viewport::RenderGrids(const Context& ctx)
{
	const Camera* active_cam = dynamic_cast<const Camera*>(ctx.scene.GetActiveCamera());

	if (active_cam == nullptr)
		return;

	ImGuizmo::DrawGrid(&active_cam->o_InvTransform[0][0], &active_cam->cam_frustum[0][0], &xdzm::identityMatrix[0][0], 30.f, 0.5f);
}

void Viewport::RenderAxis(const Context& ctx)
{
	Camera* active_cam = (Camera*)dynamic_cast<const Camera*>(ctx.scene.GetActiveCamera());

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

void Viewport::RenderHandle(const Context& ctx)
{
	Scene* scene = dynamic_cast<Scene*>(ctx.scene.active_scene);
	Camera* active_cam = dynamic_cast<Camera*>(scene->GetActiveCamera());

	const SelectionManager<ObjectID>& sel = ctx.editor.selections;
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
	
	if (hover) {
		viewport_status = HoverStatus(viewport_status | HoverStatus::OnHandle);
	}
	else {
		viewport_status = HoverStatus(viewport_status & ~HoverStatus::OnHandle);
	}

	if(click) // TODO: event system
		active_trans->SetTrans(obj_trans);
}
