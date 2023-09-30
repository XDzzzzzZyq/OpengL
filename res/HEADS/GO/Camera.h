#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "DebugMesh.h"
#include "EventListener.h"
/*#include "Sprite.h"*/

class Camera : public GameObject, public Transform3D, public EventListener
{
public:

	float cam_w, cam_h, cam_pers, cam_near, cam_far, cam_foc;
	glm::vec3 cam_tar = glm::vec3(0.0f, 0.0f, 0.0f);

	bool is_frustum_changed = true;
	glm::mat4 cam_frustum=glm::mat4(-1.0f);

public:

	Camera(float w,float h,float per,float n,float f);
	Camera();

public:

	std::vector<float> cam_floatData;
	void GenFloatData(); // 6f(trans) + 1f(ratio) + 1f(angle)
	void ChangeCamRatio(float w, float h);
	void ChangeCamRatio(const glm::vec2& size);
	void ChangeCamPersp(float persp);

	void SetTarPos(const glm::vec3& _pos);
	void SetCamTrans(const glm::mat4& _trans, bool pos = true, bool rot = true);

	void* GetTransform()	override { return dynamic_cast<Transform*>(GetTransformPtr()); }

public:

	void SHIFT_MMB();
	void CTRL_MMB();
	void ALT_MMB();
	void MMB();
	void SCROLL();

	void EventInit();
	

	~Camera();
};

