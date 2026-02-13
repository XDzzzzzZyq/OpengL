#include "CameraController.h"
#include "events/CameraEvents.h"
#include "xdz_math.h"

void CameraZoom(CameraZoomEvent e)
{
	const glm::vec3 delta = e.cam->o_position - e.cam->cam_tar;
	e.cam->SetPos(e.cam->cam_tar + delta * glm::pow(0.8f, e.scroll_dir));
}

void CameraRotate(CameraRotateEvent e)
{
	const glm::vec2 delta_mouse = glm::vec2(e.mouse_delta_x, e.mouse_delta_y);
	const glm::vec2 angle = delta_mouse * -0.01f;

	e.cam->Spin(e.cam->cam_tar, angle);
}

void CameraPush(CameraPushEvent e)
{
	e.cam->o_position += glm::cross(e.cam->o_dir_up, e.cam->o_dir_right) * xdzm::dir_float_dist(e.mouse_delta_x, e.mouse_delta_y) * 0.05f;
	e.cam->is_TransF_changed = true;
}

void CameraSpin(CameraSpinEvent e)
{
	glm::vec3 Delt_angle = glm::vec3(e.mouse_delta_y, 0.0f, e.mouse_delta_x) * 0.05f;

	e.cam->SetRot(e.cam->o_rot + Delt_angle);

	e.cam->cam_tar -= e.cam->o_position;
	e.cam->cam_tar = glm::mat4_cast(glm::qua<float>(glm::radians(Delt_angle))) * e.cam->cam_tar;
	e.cam->cam_tar += e.cam->o_position;
}

void CameraSlide(CameraSlideEvent e)
{
	const glm::vec3 delta = -e.mouse_delta_x * 0.03f * e.cam->o_dir_right + e.mouse_delta_y * 0.03f * e.cam->o_dir_up;
	e.cam->SetPos(e.cam->o_position + delta);

	e.cam->cam_tar += delta;
}

void CameraController::bind(EventPool& pool)
{
	pool.subscribe<CameraZoomEvent>(CameraZoom);
	pool.subscribe<CameraRotateEvent>(CameraRotate);
	pool.subscribe<CameraSlideEvent>(CameraSlide);
	pool.subscribe<CameraPushEvent>(CameraPush);
	pool.subscribe<CameraSpinEvent>(CameraSpin);
}