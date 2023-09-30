#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "macros.h"
#ifdef _DEBUG
#include "operator.h"
#include <iostream>
#endif

//https://glm.g-truc.net/0.9.9/api/a00356.html

// Transform Templates Lib 
class Transform {
public:
	mutable bool is_TransF_changed = true;						 // pos / scale / rot changed
	mutable bool is_invTransF_changed = true;					 //
	mutable bool is_Uniform_changed = true;						 //
	mutable bool is_invUniform_changed = true;					 //
public:
	virtual void UnsetParent(bool _keep_offset = true) = 0;
public:
	virtual bool ApplyTransform(bool _forced = false) = 0;
	virtual bool ApplyAllTransform() = 0;
	virtual bool GetInvTransform() const = 0;
public:
	virtual int Debug() const = 0;
};

// For 3D object Transform
class Transform3D : public Transform
{
private:
	Transform3D* o_parent_trans{ nullptr };
	Transform3D* o_child_trans{ nullptr };

public:
	Transform3D();
	~Transform3D();

	Transform3D* GetTransformPtr() { return this; }
	Transform3D* GetParentTransPtr() { return o_parent_trans; }
	Transform3D* GetChildTransPtr() { return o_child_trans; }

public:
	mutable glm::mat4 o_Transform{ 1.0f };
	mutable glm::mat4 o_InvTransform{ 1.0f };

	mutable glm::vec3 o_position{ 0.0f };

	mutable glm::vec3 o_scale{ 1.0f };

	mutable glm::vec3 o_rot{ 0.0f };
	mutable glm::vec3 o_dir_up{ 0.0f, 1.0f, 0.0f };
	mutable glm::vec3 o_dir_right{ 1.0f, 0.0f, 0.0f };

public:
	bool SetPos(const glm::vec3& pos);
	template <int _Dim>
	bool SetPos1D(float _1d);
	bool SetScale(const glm::vec3& scale);
	template <int _Dim>
	bool SetScale1D(float _1d);
	bool SetRot(const glm::vec3& rot);
	template <int _Dim>
	bool SetRot1D(float _1d);
	bool SetTrans(const glm::mat4& _trans, bool pos = true, bool rot = true, bool scl = true);

	void Trans(const glm::mat4& _trans);
	void Move(const glm::vec3& d_pos);
	void Spin(const glm::vec3& anch, const glm::vec3& axis, const float& angle);
	void Spin(const glm::vec3& anch, const glm::vec2& angle, bool global_up = true);
	void LookAt(const glm::vec3& tar);


	void SetParent(Transform3D* _p_trans, bool _keep_offset = true);
	void UnsetParent(bool _keep_offset = true) override;

private:
	template <int _Dim>
	bool Set1D(glm::vec3& _tar, float _1d);

	void UpdateDirections();

public:
	[[nodiscard("You could receive the state")]] bool ApplyTransform(bool _forced = false) override;
	[[nodiscard("You could receive the state")]] bool ApplyAllTransform() override;
	[[nodiscard("You could receive the state")]] bool GetInvTransform() const override;

	[[nodiscard("You could receive the state")]] int Debug() const override {
#ifdef _DEBUG
		std::cout << o_rot;
		std::cout << o_dir_up;
		std::cout << o_dir_right;
		DEBUG("________________") 
#endif // _DEBUG
		return 0;
	}

};

inline glm::mat4 OffestTransform(const glm::mat4& in_m, const glm::vec3& in_v) {
	glm::mat4 result(in_m);
	LOOP(3)
		result[3][i] += in_v[i];
	return result;
}

template <int _Dim>
bool Transform3D::Set1D(glm::vec3& _tar, float _1d)
{
	if constexpr (_Dim < 0 || _Dim > 2) return false;
	if (_tar[_Dim] == _1d) return false;

	_tar[_Dim] = _1d;

	is_TransF_changed = true;
	return true;
}

template <int _Dim>
bool Transform3D::SetPos1D(float _1d)
{
	return Set1D<_Dim>(o_position, _1d);
}

template <int _Dim>
bool Transform3D::SetScale1D(float _1d)
{
	return Set1D<_Dim>(o_scale, _1d);
}

template <int _Dim>
bool Transform3D::SetRot1D(float _1d)
{
	bool res = Set1D<_Dim>(o_rot, _1d);
	if (!res) return false;

	UpdateDirections();
	return true;
}







// For 2D object Transform
class Transform2D : public Transform
{
private:
	Transform2D* o_parent_trans{ nullptr };
	Transform2D* o_child_trans{ nullptr };
public:
	Transform2D();
	~Transform2D();

	Transform2D* GetTransformPtr() { return this; }
	Transform2D* GetParentTransPtr() { return o_parent_trans; }
	Transform2D* GetChildTransPtr() { return o_child_trans; }
public:
	mutable glm::mat3 o_Transform{ 1.0f };
	mutable glm::mat3 o_InvTransform{ 1.0f };

	mutable glm::vec2 o_position{ 0.0f };

	mutable glm::vec2 o_scale{ 1.0f };

	mutable float o_rot{ 0.0f };

public:
	bool SetPos(const glm::vec2& pos);
	bool SetScale(const glm::vec2& scale);
	bool SetRot(float rot);

	void Trans(const glm::mat3& _trans);
	void Move(const glm::vec2& d_pos);
	void Spin(float angle);
	void Zoom(const glm::vec2& scale);
	void Zoom(float scale);
	void LookAt(const glm::vec2& tar);\

	void SetParent(Transform2D* _p_trans, bool _keep_offset = true);
	void UnsetParent(bool _keep_offset = true) override;
public:
	[[nodiscard("You can receive the state")]] bool ApplyTransform(bool _forced = false) override;
	[[nodiscard("You can receive the state")]] bool ApplyAllTransform() override;
	[[nodiscard("You can receive the state")]] bool GetInvTransform() const override;

	[[nodiscard("You can receive the state")]] int Debug() const override {
#ifdef _DEBUG
		std::cout << o_rot;
		DEBUG("________________") 
#endif // _DEBUG
		return 0;
	}

};