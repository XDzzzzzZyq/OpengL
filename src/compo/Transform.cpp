#include "Transform.h"
#include "xdz_math.h"

#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "macros.h"
#include "operator.h"

inline glm::mat4 OffestTransform(const glm::mat4& in_m, const glm::vec3& in_v) {
	glm::mat4 result(in_m);
	LOOP(3)
		result[3][i] += in_v[i];
	return result;
}

void Transform::UseTranformComp(bool _enable, TransType _type)
{
	if (_type & TransType::Position)
		use_position = _enable;
	if (_type & TransType::Rotation)
		use_rotation = _enable;
	if (_type & TransType::Scale)
		use_scale = _enable;
}


Transform3D::Transform3D()
{

}

Transform3D::~Transform3D()
{

}

bool Transform3D::SetPos(const glm::vec3& pos)
{
	if (o_position == pos) return false;
	if (!use_position) return false;

	is_TransF_changed = true;
	o_position = pos;

	return true;
}

bool Transform3D::SetPos1D(float _1d, GLuint _dim)
{
	if (!use_position) return false;
	return Set1D(o_position, _1d, _dim);
}

bool Transform3D::SetScale(const glm::vec3& scale)
{
	if (o_scale == scale) return false;
	if (!use_scale) return false;

	is_TransF_changed = true;
	o_scale = scale;
	
	return true;
}

bool Transform3D::SetScale1D(float _1d, GLuint _dim)
{
	if (!use_scale) return false;
	return Set1D(o_scale, _1d, _dim);
}

bool Transform3D::SetRot(const glm::vec3& rot)
{
	if (o_rot == rot) return false;
	if (!use_rotation) return false;

	is_TransF_changed = true;
	o_rot = rot;

	UpdateDirections();

	return true;
}

bool Transform3D::SetRot1D(float _1d, GLuint _dim)
{

	if (!use_rotation) return false;
	bool res = Set1D(o_rot, _1d, _dim);
	if (!res) return false;

	UpdateDirections();
	return true;
}

bool Transform3D::SetTrans(const glm::mat4& _trans, bool pos /*= true*/, bool rot /*= true*/, bool scl /*= true*/)
{
	if (_trans == o_Transform)
		return false;

	glm::vec3 scale, position, _sk;
	glm::quat rotation;
	glm::vec4 _pers;

	glm::decompose(_trans, scale, rotation, position, _sk, _pers);

	if (pos) SetPos(position);
	if (rot) SetRot(glm::degrees(glm::eulerAngles(rotation)));
	if (scl) SetScale(scale);
	if (pos && rot && scl) 
		if(use_position && use_rotation && use_scale) // if there is at least one lock, then let calcluate transform instead
			o_Transform = _trans;

	return true;
}

void Transform3D::Trans(const glm::mat4& _trans)
{
	is_invTransF_changed = true;
	is_invUniform_changed = true;

	o_Transform = _trans * o_Transform;
}

void Transform3D::Move(const glm::vec3& d_pos)
{
	assert(false);
}

void Transform3D::Spin(const glm::vec3& anch, const glm::vec3& axis, const float& angle)
{
	assert(false);
}

void Transform3D::Spin(const glm::vec3& anch, const glm::vec2& angle, bool global_up/* = true*/)
{
	if(angle == glm::vec2(0))
		return;

	o_position -= anch;

	o_position = global_up ? glm::rotateZ(o_position, angle.x) : glm::rotate(o_position, angle.y, o_dir_up);
	SetRot(o_rot + glm::vec3(0.0f, 0.0f, glm::degrees(angle.x)));

	o_position = glm::rotate(o_position, angle.y, o_dir_right);
	SetRot(o_rot + glm::vec3(glm::degrees(angle.y), 0.0f, 0.0f));
	
	o_position += anch;
}

void Transform3D::LookAt(const glm::vec3& tar)
{
	assert(false);
}

void Transform3D::SetParent(Transform3D* _p_trans, bool _keep_offset /*= true*/)
{
	ApplyTransform();
	_p_trans->ApplyTransform();
	_p_trans->GetInvTransform();

	o_parent_trans = _p_trans;
	_p_trans->o_child_trans = this;

	if (!_keep_offset) return;

	glm::vec3 pos_off = o_position - _p_trans->o_position;
	// if self.trans = B=DA, parent.trans = A, then D=BA^-1
	glm::mat4 D = o_Transform * _p_trans->o_InvTransform;
	SetTrans(D);
	SetPos(D*glm::vec4(pos_off, 0));
}

void Transform3D::UnsetParent(bool _keep_offset /*= true*/)
{

}

bool Transform3D::Set1D(glm::vec3& _tar, float _1d, GLuint _dim)
{
	if (_dim < 0 || _dim > 2) return false;
	if (_tar[_dim] == _1d) return false;

	_tar[_dim] = _1d;

	is_TransF_changed = true;
	return true;
}

void Transform3D::UpdateDirections()
{
	glm::mat4 rot_mat = glm::mat4_cast(glm::qua(glm::radians(o_rot)));

	o_dir_up = rot_mat * glm::vec3(0.0f, 1.0f, 0.0f);
	o_dir_right = rot_mat * glm::vec3(1.0f, 0.0f, 0.0f);
}

bool Transform3D::ApplyTransform(bool _forced /*= false*/)
{

	if (!is_TransF_changed && !_forced) return false;

	glm::mat4 rot_mat = glm::mat4_cast(glm::qua(glm::radians(o_rot)));
	o_Transform = rot_mat * glm::scale(glm::mat4(1), o_scale);
	o_Transform = OffestTransform(o_Transform, o_position);

	is_invTransF_changed = true;
	is_TransF_changed = false;
	is_Uniform_changed = true;

	return true;

}

bool Transform3D::ApplyAllTransform()
{
	if (GetParentTransPtr() != nullptr) { // skip all child node 
		return false;
	}

	Transform3D* tar_ptr = this;

	glm::mat4 post_trans = glm::mat4(1);
	bool is_changed = false;
	
	do {
		is_changed |= tar_ptr->is_TransF_changed;
		if (is_changed) {
			tar_ptr->ApplyTransform(true);
			tar_ptr->o_Transform = post_trans * tar_ptr->o_Transform;
		}

		tar_ptr->is_Uniform_changed = is_changed;
		post_trans = tar_ptr->o_Transform;
		
		if (tar_ptr->GetChildTransPtr() == nullptr)
			break;
		else 
			tar_ptr = tar_ptr->GetChildTransPtr();
	} while (true);

	return true;

}

/*std::unordered_map<int, int, float> */

bool Transform3D::GetInvTransform() const
{
	if (!is_invTransF_changed) return false;

	glm::mat3 inv_rot = glm::mat3(o_Transform);
	LOOP(3) inv_rot[i] = glm::normalize(inv_rot[i]);
	inv_rot = glm::transpose(inv_rot);
	glm::mat4 result = glm::mat4(inv_rot) * glm::scale(glm::mat4(1), 1.0f/o_scale);
	//std::cout << result;
	o_InvTransform = glm::translate(result, -o_position);

	//o_InvTransform[3][3] = 1.0f;
	is_invTransF_changed = false;
	//std::cout << o_InvTransform;
	//std::cout << o_Transform;
	//DEBUG("+++++++++++++++++++++++++++++++")
	is_invUniform_changed = true;

	return true;
}

int Transform3D::_debug() const
{
#ifdef _DEBUG
	std::cout << o_rot << ", ";
	std::cout << o_dir_up << ", ";
	std::cout << o_dir_right << ", ";
	DEBUG("________________")
#endif // _DEBUG
		return 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



Transform2D::Transform2D()
{

}

Transform2D::~Transform2D()
{

}

bool Transform2D::SetPos(const glm::vec2& pos)
{
	if (pos == o_position) return false;
	o_position = pos;
	is_TransF_changed = true;

	return true;
}

bool Transform2D::SetScale(const glm::vec2& scale)
{
	if (scale == o_scale) return false;
	o_scale = scale;
	is_TransF_changed = true;

	return true;
}

bool Transform2D::SetRot(float rot)
{
	if (rot == o_rot) return false;
	o_rot = rot;
	is_TransF_changed = true;

	return true;
}

void Transform2D::Trans(const glm::mat3& _trans)
{

}

void Transform2D::Move(const glm::vec2& d_pos)
{
	if (d_pos != glm::vec2(0))
		SetPos(o_position + d_pos);
}

void Transform2D::Spin(float angle)
{
	if (angle != 0)
		SetRot(o_rot + angle);
}

void Transform2D::Zoom(const glm::vec2& scale)
{
	if (scale != glm::vec2(0) && scale != glm::vec2(1))
		SetScale(o_scale * scale);

}

void Transform2D::Zoom(float scale)
{
	if (scale != 1 && scale != 0)
		SetScale(o_scale * scale);

}

void Transform2D::LookAt(const glm::vec2& tar)
{

}

void Transform2D::SetParent(Transform2D* _p_trans, bool _keep_offset /*= true*/)
{

}

void Transform2D::UnsetParent(bool _keep_offset /*= true*/)
{

}

bool Transform2D::ApplyTransform(bool _forced /*= false*/)
{
	if (!is_TransF_changed) return false;

	LOOP(2) {
		o_Transform[i][i] = o_scale[i];
		o_Transform[2][i] = o_position[i] * o_scale[i];
	}
	o_Transform[2][2] = 1.0f;

	is_TransF_changed = false;
	is_invTransF_changed = true;

	return true;
}

bool Transform2D::ApplyAllTransform()
{
	return true;
}

bool Transform2D::GetInvTransform() const
{
	if (!is_invTransF_changed) return false;

	LOOP(2) {
		o_InvTransform[i][i] = 1 / o_scale[i];
		o_InvTransform[2][i] = -o_position[i] / o_scale[i];
	}
	o_InvTransform[2][2] = 1.0f;

	is_invTransF_changed = false;

	return true;
}

int Transform2D::_debug() const
{
#ifdef _DEBUG
	std::cout << o_rot;
	DEBUG("________________")
#endif // _DEBUG
		return 0;
}
