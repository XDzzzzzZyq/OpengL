#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "DebugMesh.h"
#include "EventListener.h"
/*#include "Sprite.h"*/

#include "support.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera : public GameObject, public Transform3D, public EventListener
{
private:


public:
	float cam_w, cam_h, cam_pers, cam_near, cam_far, cam_foc;
	glm::vec3 cam_tar = glm::vec3(0.0f, 0.0f, 0.0f);

	bool is_frustum_changed = true;

	glm::mat4 cam_frustum=glm::mat4(-1.0f);
	Camera(float w,float h,float per,float n,float f);
	Camera();

	float* GetCameraParas() const;
	mutable std::vector<float> cam_floatData;
	void GenFloatData() const; // 6f(trans) + 1f(ratio) + 1f(angle)
	void ChangeCamRatio(float w, float h);
	void ChangeCamRatio(const ImVec2& size);
	void ChangeCamPersp(float persp);

	void SHIFT_MMB();
	void CTRL_MMB();
	void ALT_MMB();
	void MMB();
	void SCROLL();

	void EventInit();
	

	~Camera();
};

