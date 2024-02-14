#include "Camera.h"
#include "xdz_math.h"
#include "xdz_matrix.h"
#include "glm/gtx/matrix_decompose.hpp"

Camera::Camera(float w, float h, float per, float n, float f)
	:cam_w(w), cam_h(h), cam_pers(per), cam_near(n), cam_far(f)
{
	o_type = GO_CAM;
	//cam_frustum = glm::ortho(-cam_w / 2, cam_w / 2, -cam_h / 2, cam_h / 2, cam_near, cam_far);
	cam_frustum = glm::perspective(glm::radians(per), w / h, n, f); //fov angle from y axis

	o_name = "Camera." + std::to_string(GetObjectID());

	EventInit();
}

Camera::Camera()
{
	o_type = GO_CAM;
	cam_w = 0;
	cam_h = 0;
}

void Camera::EventInit()
{
	EventList[GenIntEvent(0, 0, 0, 3, 0)] = REGIST_EVENT(Camera::MMB);
	EventList[GenIntEvent(1, 0, 0, 3, 0)] = REGIST_EVENT(Camera::SHIFT_MMB);
	EventList[GenIntEvent(2, 0, 0, 3, 0)] = REGIST_EVENT(Camera::CTRL_MMB);
	EventList[GenIntEvent(3, 0, 0, 3, 0)] = REGIST_EVENT(Camera::ALT_MMB);
	EventList[GenIntEvent(0, 0, 0, 0, 1)] = REGIST_EVENT(Camera::SCROLL);
	EventList[GenIntEvent(0, 0, 0, 0, -1)] = REGIST_EVENT(Camera::SCROLL);
}

Camera::~Camera()
{

}

void Camera::GenFloatData()
{
	cam_floatData = {
		o_position[0],
		o_position[1],
		o_position[2],

		o_rot[0],
		o_rot[1],
		o_rot[2],

		cam_w / cam_h,              //ratio
		glm::radians(cam_pers)
	};
}

void Camera::ChangeCamRatio(float w, float h)
{
	if (w == cam_w && h == cam_h)
		return;

	cam_w = w;
	cam_h = h;

	cam_frustum = glm::perspective(glm::radians(cam_pers), cam_w / cam_h, cam_near, cam_far);
	is_frustum_changed = true;
}

void Camera::ChangeCamRatio(const glm::vec2& size)
{
	if (size == glm::vec2(cam_w, cam_h))
		return;

	cam_w = size.x;
	cam_h = size.y;

	cam_frustum = glm::perspective(glm::radians(cam_pers), cam_w / cam_h, cam_near, cam_far);
	is_frustum_changed = true;
}

void Camera::ChangeCamPersp(float persp)
{
	if (persp == cam_pers)
		return;

	cam_pers = persp;
	cam_frustum = glm::perspective(glm::radians(cam_pers), cam_w / cam_h, cam_near, cam_far);
	is_frustum_changed = true;
}

void Camera::SetCamPos(const glm::vec3& _pos)
{
	if (_pos == o_position)
		return;

	o_position = _pos;
	glm::mat4 trans = xdzm::lookAt(o_position, cam_tar, glm::vec3(0,0,1));
	//glm::vec3 dir = glm::normalize(cam_tar - o_position);
	SetTrans(trans, false, true, false);
}

void Camera::SetTarPos(const glm::vec3& _pos)
{
	if (_pos == cam_tar)
		return;

	cam_tar = _pos;
	glm::mat4 trans = xdzm::lookAt(o_position, cam_tar, glm::vec3(0, 0, 1));
	//glm::vec3 dir = glm::normalize(cam_tar - o_position);
	SetTrans(trans, false, true, false);
}

void Camera::SetCamTrans(const glm::mat4& _trans, bool pos /*= true*/, bool rot /*= true*/)
{
	if (xdzm::matrix_diff(_trans, o_Transform) < 0.000001)
		return;

	glm::vec3 _s, position, _sk;
	glm::quat rotation;
	glm::vec4 _per;

	glm::decompose(_trans, _s, rotation, position, _sk, _per);

	if (pos)
	{ 
		cam_tar += position - o_position;
		SetPos(position);
	}

	if (rot)
	{ 
		const glm::vec3 delta = glm::eulerAngles(rotation) - glm::radians(o_rot);
		//DEBUG(delta);
		Spin(cam_tar, { delta.z, delta.x });
		//SetRot(glm::degrees(glm::eulerAngles(rotation)));
	}
}

void Camera::SHIFT_MMB()
{
	if (!EventListener::is_in_viewport) return;

	const glm::vec3 delta = -(float)(mouse_x - mouse_b_x) * 0.03f * o_dir_right + (float)(mouse_y - mouse_b_y) * 0.03f * o_dir_up;
	SetPos(o_position + delta);

	cam_tar += delta;
}

void Camera::CTRL_MMB()
{
	if (!EventListener::is_in_viewport) return;

	o_position += glm::cross(o_dir_up, o_dir_right) * xdzm::dir_float_dist((float)(mouse_x - mouse_b_x), (float)(mouse_y - mouse_b_y)) * 0.05f;
	is_TransF_changed = true;
}

void Camera::ALT_MMB()
{
	if (!EventListener::is_in_viewport) return;

	glm::vec3 Delt_angle = glm::vec3(EventListener::GetDeltaMouseY(), 0.0f, EventListener::GetDeltaMouseX()) * 0.05f;

	SetRot(o_rot + Delt_angle);

	cam_tar -= o_position;
	cam_tar = glm::mat4_cast(glm::qua<float>(glm::radians(Delt_angle))) * cam_tar;
	cam_tar += o_position;
}

void Camera::MMB()
{
	if (!EventListener::is_in_viewport) return;

	const glm::vec2 angle =	EventListener::GetDeltaMouse() * -0.01f;

	Spin(cam_tar, angle);
}

void Camera::SCROLL()
{
	if (!EventListener::is_in_viewport)return;

	const glm::vec3 delta = o_position - cam_tar;
	SetPos(cam_tar + delta * glm::pow(0.8f, EventListener::scroll_dir));
}

