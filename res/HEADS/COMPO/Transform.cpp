#include "Transform.h"

Transform::Transform()
{

}

Transform::~Transform()
{

}

void Transform::SetPos(const glm::vec3& pos)
{
	if (o_position != pos)
	{
		is_TransF_changed = true;
		o_Transform = glm::translate(o_Transform, -o_position);
		o_position = pos;
		/*ApplyTransform();*/
		o_Transform = glm::translate(o_Transform, o_position);
	}

}

void Transform::SetScale(const glm::vec3& scale)
{
	if (o_scale != scale)
	{
		is_TransF_changed = true;
		o_scale = scale;
		/*		ApplyTransform();*/
	}
}

void Transform::SetRot(const glm::vec3& rot)
{
	is_rot_changed = false;
	if (o_rot != rot)
	{
		is_TransF_changed = true;
		is_rot_changed = true;
		o_rot = rot;
		rotQua = glm::qua(glm::radians(o_rot));

		/*		ApplyTransform();*/
	}
}

void Transform::Trans(const glm::mat4& _trans)
{
	is_invTransF_changed = true;
	is_invUniform_changed = true;

	o_Transform = _trans * o_Transform;
}

void Transform::Move(const glm::vec3& d_pos)
{
	is_TransF_changed = true;
	o_Transform = glm::translate(o_Transform, -o_position);
	o_position += d_pos;
	/*ApplyTransform();*/
	o_Transform = glm::translate(o_Transform, o_position);
}

void Transform::Spin(const glm::vec3& anch, const glm::vec3& axis, const float& angle)
{
	is_invTransF_changed = true;
	rotQua = glm::qua<float>(glm::radians(o_rot));
	rotQua = glm::rotate(rotQua, angle, axis);
	o_rotMat = glm::mat4_cast(rotQua);
	o_Transform = glm::scale(glm::mat4(1.0f), o_scale);

	o_position -= anch;
	o_position = anch + o_rotMat * o_position;
		
	o_Transform = glm::translate(o_Transform, o_position);

	o_dir_up = o_rotMat * glm::vec3(0.0f, 1.0f, 0.0f);
	o_dir_right = o_rotMat * glm::vec3(1.0f, 0.0f, 0.0f);
}

void Transform::LookAt(const glm::vec3& tar)
{
	is_invTransF_changed = true;
	is_TransF_changed = false;
	
	//rotMat = glm::lookAt(o_position, tar, o_dir_up);
	rotQua = glm::quatLookAt(glm::normalize(tar - o_position), o_dir_up);
	o_rotMat = glm::mat4_cast(rotQua);
	
	o_dir_up = o_rotMat * glm::vec3(0.0f, 1.0f, 0.0f);
	o_dir_right = o_rotMat * glm::vec3(1.0f, 0.0f, 0.0f);

	o_Transform = glm::scale(glm::mat4(1.0f), o_scale);

	o_Transform = glm::translate(o_Transform, o_position);
}

void Transform::SetParent(Transform* _p_trans, bool _keep_offset /*= true*/)
{
	o_parent_trans = _p_trans;
	_p_trans->o_child_trans = this;

	SetScale(o_scale / _p_trans->o_scale);
	SetPos((o_position - _p_trans->o_position)*o_scale);
	ApplyTransform();
}

bool Transform::ApplyTransform()
{

	if (is_TransF_changed)
	{
		o_rotMat = glm::mat4_cast(rotQua);
		o_Transform = o_rotMat * glm::scale(glm::mat4(1), o_scale);
		o_Transform = OffestTransform(o_Transform, o_position);

		if (is_rot_changed)
		{
			o_dir_up = o_rotMat * glm::vec3(0.0f, 1.0f, 0.0f);
			o_dir_right = o_rotMat * glm::vec3(1.0f, 0.0f, 0.0f);
			is_rot_changed = false;

		}
		is_invTransF_changed = true;
		is_TransF_changed = false;
		is_Uniform_changed = true;

		return true;
	}
	return false;
}

void Transform::ApplyAllTransform()
{
	if (o_parent_trans) {
		Transform* tar_ptr = this;
		int i = 0;
		do{
			tar_ptr->ApplyTransform();
			i++;
			if (tar_ptr->GetParentTransPtr() == nullptr)
				break;
			else
				tar_ptr = tar_ptr->GetParentTransPtr();
			
		} while (true);
		glm::mat4 tar_trans(1.0f);
		do{
			if (tar_ptr == nullptr)
				break;
			tar_trans = tar_ptr->o_Transform = tar_trans * tar_ptr->o_Transform;
			if (tar_ptr->GetChildTransPtr() == nullptr)
				break;
			else
				tar_ptr = tar_ptr->GetChildTransPtr();
		}while(true);
	}
	else {
		ApplyTransform();
	}
}

/*std::unordered_map<int, int, float> */

glm::mat4 Transform::GetInvTransform() const
{
	if (is_invTransF_changed)
	{
		glm::mat3 result = glm::mat3(o_Transform);
		//std::cout << result;
		o_InvTransform = glm::translate(glm::transpose(glm::mat4(result)), -1.0f * o_position);

		//o_InvTransform[3][3] = 1.0f;
		is_invTransF_changed = false;
		//std::cout << o_InvTransform;
		//std::cout << o_Transform;
		//DEBUG("+++++++++++++++++++++++++++++++")
		is_invUniform_changed = true;
		return o_InvTransform;
	}
	return o_InvTransform;
}