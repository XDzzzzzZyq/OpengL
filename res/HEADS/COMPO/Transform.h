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
	glm::mat4 rotMat;
	glm::qua<float> rotQua;
public:
	Transform();
	~Transform();


	mutable bool is_TransF_changed = true;
	mutable bool is_invTransF_changed = true;
	mutable bool is_rot_changed = true;

	mutable glm::mat4 o_Transform = glm::mat4(1.0f);
	mutable glm::mat4 o_InvTransform = glm::mat4(1.0f);

	mutable glm::vec3 o_position = glm::vec3(0.0f);

	mutable glm::vec3 o_scale = glm::vec3(1.0f);

	mutable glm::vec3 o_rot = glm::vec3(0.0f);
	mutable glm::vec3 o_dir_up = glm::vec3(0.0f, 1.0f, 0.0f);
	mutable glm::vec3 o_dir_right = glm::vec3(1.0f, 0.0f, 0.0f);

	void SetPos(const glm::vec3& pos);
	void SetScale(const glm::vec3& scale);
	void SetRot(const glm::vec3& rot);

	void Move(const glm::vec3& d_pos);
	void Spin(const glm::vec3& anch, const glm::vec3& axis, const float& angle);
	void LookAt(const glm::vec3& tar);
	//glm::rotate(vec<3, T, Q> const& v, T const& angle, vec<3, T, Q> const& normal)


	void Normalize() {
		o_rot = glm::normalize(o_rot);
	}

	void ApplyTransform();
	glm::mat4 GetInvTransform() const;

	void PrintTransState() {
		//std::cout << is_TransF_changed << " " << is_invTransF_changed << " " << is_rot_changed << "\n";
		std::cout << o_rot;
		std::cout << o_dir_up;
		std::cout << o_dir_right;
		DEBUG("________________")
	}

};

