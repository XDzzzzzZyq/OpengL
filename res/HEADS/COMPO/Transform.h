#pragma once
#include "support.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"

//https://glm.g-truc.net/0.9.9/api/a00356.html

// Transform Templates Lib 
class Transform {
public:
	mutable bool is_TransF_changed = true;						 //pos / scale / rot changed
	mutable bool is_invTransF_changed = true;					 //
	mutable bool is_rot_changed = true;							 //
	mutable bool is_Uniform_changed = false;					 //
	mutable bool is_invUniform_changed = false;					 //
public:
	virtual void UnsetParent(bool _keep_offset = true) = 0;
public:
	virtual bool ApplyTransform() = 0;
	virtual void ApplyAllTransform() = 0;
};

// For 3D object Transform
class Transform3D : public Transform
{
private:
	Transform3D* o_parent_trans{ nullptr };
	Transform3D* o_child_trans{ nullptr };
public:
	glm::mat4 o_rotMat;
	glm::qua<float> rotQua;
public:
	Transform3D();
	~Transform3D();

	Transform3D* GetTransformPtr() { return this; }
	Transform3D* GetParentTransPtr() { return o_parent_trans; }
	Transform3D* GetChildTransPtr() { return o_child_trans; }
public:
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

	void SetParent(Transform3D* _p_trans, bool _keep_offset = true);
	void UnsetParent(bool _keep_offset = true) override;
public:
	bool ApplyTransform() override;
	void ApplyAllTransform() override;
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





// For 2D object Transform
class Transform2D : public Transform
{
private:
	Transform2D* o_parent_trans{ nullptr };
	Transform2D* o_child_trans{ nullptr };
public:
	glm::mat3 o_rotMat;
public:
	Transform2D();
	~Transform2D();

	Transform2D* GetTransformPtr() { return this; }
	Transform2D* GetParentTransPtr() { return o_parent_trans; }
	Transform2D* GetChildTransPtr() { return o_child_trans; }
public:
	mutable glm::mat3 o_Transform = glm::mat3(1.0f);
	mutable glm::mat3 o_InvTransform = glm::mat3(1.0f);

	mutable glm::vec2 o_position = glm::vec2(0.0f);

	mutable glm::vec2 o_scale = glm::vec2(1.0f);

	mutable float o_rot = 0;
	mutable glm::vec2 o_dir_up = glm::vec2(0.0f, 1.0f);
	mutable glm::vec2 o_dir_right = glm::vec2(1.0f, 0.0f);

public:
	void SetPos(const glm::vec2& pos);
	void SetScale(const glm::vec2& scale);
	void SetRot(const glm::vec2& rot);

	void Trans(const glm::mat3& _trans);
	void Move(const glm::vec2& d_pos);
	void Spin(const glm::vec2& anch, const glm::vec3& axis, const float& angle);
	void LookAt(const glm::vec2& tar);


	void SetParent(Transform2D* _p_trans, bool _keep_offset = true);
	void UnsetParent(bool _keep_offset = true) override;
public:
	bool ApplyTransform() override;
	void ApplyAllTransform() override;
	glm::mat3 GetInvTransform() const;

	void PrintTransState() {
		//std::cout << is_TransF_changed << " " << is_invTransF_changed << " " << is_rot_changed << "\n";
		std::cout << o_rot;
		//std::cout << o_dir_up;
		//std::cout << o_dir_right;
		DEBUG("________________")
	}

};