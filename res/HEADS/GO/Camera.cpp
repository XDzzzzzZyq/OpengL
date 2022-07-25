#include "Camera.h"
#include "support.h"


Camera::Camera(float w, float h, float per, float n, float f)
	:cam_w(w), cam_h(h), cam_pers(per), cam_near(n), cam_far(f)
{
	//cam_frustum = glm::ortho(-cam_w / 2, cam_w / 2, -cam_h / 2, cam_h / 2, cam_near, cam_far);
	cam_frustum = glm::perspective(glm::radians(per), w / h, n, f); //fov angle from y axis

	EventInit();
	o_name = "Camera." + std::to_string(GetObjectID());
}

Camera::Camera()
{
	DEBUG("cam c")
		cam_w = 0;
	cam_h = 0;
}

void Camera::EventInit()
{
	EventList[GenIntEvent(0, 0, 0, 2, 0, 0)] = std::bind(&Camera::MMB, this);
	EventList[GenIntEvent(1, 0, 0, 2, 0, 0)] = std::bind(&Camera::SHIFT_MMB, this);
	EventList[GenIntEvent(2, 0, 0, 2, 0, 0)] = std::bind(&Camera::CTRL_MMB, this);
	EventList[GenIntEvent(3, 0, 0, 2, 0, 0)] = std::bind(&Camera::ALT_MMB, this);
}

Camera::~Camera()
{

}

float* Camera::GetCameraParas() const
{
	float paras[6];
	return paras;
}

void Camera::GenFloatData() const
{
	cam_floatData = {
		o_position[0],
		o_position[1],
		o_position[2],

		o_rot[0],
		o_rot[1],
		o_rot[2],

		cam_w / cam_h,
		glm::radians(cam_pers)
	};
}

void Camera::CameraEventActivate(GLFWwindow* window)
{

	//std::cout << scroll_x << "," << scroll_y << "\n";
	UpdateEvent(window, evt_IDlist);
	//if (evt_KM.GenStateDara() != 0)
		if (EventList.find(evt_KM) != EventList.end())
			EventList[evt_KM]();
	//DEBUG(evt_IDlist.size())

	//locol
// 	switch (evt_KM.FirstKey)
// 	{
// 
// 	case 1:
// 		switch (evt_KM.mouse)
// 		{
// 		case -1:
// 			break;
// 		case 0:
// 
// 			break;
// 		case 1:
// 
// 			break;
// 		case 2:
// 			//DEBUG("SHIFT + MMB")
			//SHIFT_MMB();
// 		}
// 		break;
// 	case 2:
// 		switch (ListenMouseEvent(window))
// 		{
// 		case -1:
// 			break;
// 		case 0:
// 
// 			break;
// 		case 1:
// 
// 			break;
// 		case 2:
// 			//DEBUG("CTRL + MMB")
//			CTRL_MMB();
// 		}
// 		break;
// 	case 3:
// 		switch (ListenMouseEvent(window))
// 		{
// 		case -1:
// 			break;
// 		case 0:
// 
// 			break;
// 		case 1:
// 
// 			break;
// 		case 2:
// 			//DEBUG("ALT + MMB")
// 			ALT_MMB();
// 		}
// 		break;
// 	case 0:
// 		switch (ListenMouseEvent(window))
// 		{
// 		case -1:
// 			break;
// 		case 0:
// 
// 			break;
// 		case 1:
// 
// 			break;
// 		case 2:
// 			//DEBUG("MMB")
// 			MMB();
// 		}
// 		break;
// 	}
}

void Camera::SHIFT_MMB()
{
	o_position += -(float)(mouse_x - mouse_b_x) * 0.03f * o_dir_right + (float)(mouse_y - mouse_b_y) * 0.03f * o_dir_up;
	cam_tar += -(float)(mouse_x - mouse_b_x) * 0.03f * o_dir_right + (float)(mouse_y - mouse_b_y) * 0.03f * o_dir_up;
	is_TransF_changed = true;
}

void Camera::CTRL_MMB()
{
	o_position += glm::cross(o_dir_up, o_dir_right) * dir_float_dist((float)(mouse_x - mouse_b_x), (float)(mouse_y - mouse_b_y)) * 0.05f;
	is_TransF_changed = true;
}

void Camera::ALT_MMB()
{
	glm::vec3 Delt_angle = glm::vec3((float)(mouse_y - mouse_b_y) * 0.05f, (float)(mouse_x - mouse_b_x) * 0.05f, 0.0f);
	is_rot_changed = true;
	is_TransF_changed = true;
	SetRot(o_rot + Delt_angle);

	cam_tar -= o_position;
	cam_tar = glm::mat4_cast(glm::qua<float>(glm::radians(Delt_angle))) * cam_tar + o_position;
}

void Camera::MMB()
{
	o_position -= cam_tar;

	o_position = glm::rotateY(o_position, -(float)(mouse_x - mouse_b_x) * 0.01f);
	SetRot(o_rot + glm::vec3(0.0f, glm::degrees(-(float)(mouse_x - mouse_b_x) * 0.01f), 0.0f));

	o_position = glm::rotate(o_position, -(float)(mouse_y - mouse_b_y) * 0.01f, o_dir_right);
	SetRot(o_rot + glm::vec3(glm::degrees(-(float)(mouse_y - mouse_b_y) * 0.01f), 0.0f, 0.0f));

	o_position += cam_tar;


	is_TransF_changed = true;
}

