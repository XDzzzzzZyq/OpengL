#pragma once
#include "support.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"

//https://glm.g-truc.net/0.9.9/api/a00356.html

class Transform
{
private:
	Transform* o_parent_trans{ nullptr };
	Transform* o_child_trans{ nullptr };
public:
	glm::mat4 o_rotMat;
	glm::qua<float> rotQua;
public:
	Transform();
	~Transform();

	Transform* GetTransformPtr() { return this; }
	Transform* GetParentTransPtr() { return o_parent_trans; }
	Transform* GetChildTransPtr() { return o_child_trans; }
public:
	mutable bool is_TransF_changed = true;						 //pos / scale / rot changed
	mutable bool is_invTransF_changed = true;					 //
	mutable bool is_rot_changed = true;							 //
	mutable bool is_Uniform_changed = false;					 //
	mutable bool is_invUniform_changed = false;					 //

	mutable glm::mat4 o_Transform = glm::mat4(1.0f);
	mutable glm::mat4 o_InvTransform = glm::mat4(1.0f);

	mutable glm::vec3 o_position = glm::vec3(0.0f);

	mutable glm::vec3 o_scale = glm::vec3(1.0f);

	mutable glm::vec3 o_rot = glm::vec3(0.0f);
	mutable glm::vec3 o_dir_up = glm::vec3(0.0f, 1.0f, 0.0f);
	mutable glm::vec3 o_dir_right = glm::vec3(1.0f, 0.0f, 0.0f);

public:
	void SetPos(const glm::vec3& pos);
	void SetScale(const glm::vec3& scale);
	void SetRot(const glm::vec3& rot);

	void Trans(const glm::mat4& _trans);
	void Move(const glm::vec3& d_pos);
	void Spin(const glm::vec3& anch, const glm::vec3& axis, const float& angle);
	void LookAt(const glm::vec3& tar);

	void Normalize() { o_rot = glm::normalize(o_rot); }

	void SetParent(Transform* _p_trans, bool _keep_offset = true);
	void UnsetParent(bool _keep_offset = true);
public:
	bool ApplyTransform();
	void ApplyAllTransform();
	glm::mat4 GetInvTransform() const;

	void PrintTransState() {
		//std::cout << is_TransF_changed << " " << is_invTransF_changed << " " << is_rot_changed << "\n";
		std::cout << o_rot;
		std::cout << o_dir_up;
		std::cout << o_dir_right;
		DEBUG("________________")
	}

};

inline glm::mat4 OffestTransform(const glm::mat4& in_m, const glm::vec3& in_v) {
	glm::mat4 result(in_m);
	LOOP(3)
		result[3][i] += in_v[i];
	return result;
}
