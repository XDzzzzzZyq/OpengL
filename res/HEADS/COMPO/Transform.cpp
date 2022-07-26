#include "Transform.h"

Transform3D::Transform3D()
{

}

Transform3D::~Transform3D()
{

}

void Transform3D::SetPos(const glm::vec3& pos)
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

void Transform3D::SetScale(const glm::vec3& scale)
{
	if (o_scale != scale)
	{
		is_TransF_changed = true;
		o_scale = scale;
		/*		ApplyTransform();*/
	}
}

void Transform3D::SetRot(const glm::vec3& rot)
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

void Transform3D::Trans(const glm::mat4& _trans)
{
	is_invTransF_changed = true;
	is_invUniform_changed = true;

	o_Transform = _trans * o_Transform;
}

void Transform3D::Move(const glm::vec3& d_pos)
{
	is_TransF_changed = true;
	o_Transform = glm::translate(o_Transform, -o_position);
	o_position += d_pos;
	/*ApplyTransform();*/
	o_Transform = glm::translate(o_Transform, o_position);
}

void Transform3D::Spin(const glm::vec3& anch, const glm::vec3& axis, const float& angle)
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

void Transform3D::LookAt(const glm::vec3& tar)
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

void Transform3D::SetParent(Transform3D* _p_trans, bool _keep_offset /*= true*/)
{
	o_parent_trans = _p_trans;
	_p_trans->o_child_trans = this;

	SetScale(o_scale / _p_trans->o_scale);
	SetPos((o_position - _p_trans->o_position)*o_scale);
	ApplyTransform();
}

void Transform3D::UnsetParent(bool _keep_offset /*= true*/)
{

}

bool Transform3D::ApplyTransform()
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

bool Transform3D::ApplyAllTransform()
{
	if (o_parent_trans) {
		Transform3D* tar_ptr = this;
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
		return true;
	}
	else {
		ApplyTransform();
		return false;
	}
}

/*std::unordered_map<int, int, float> */

bool Transform3D::GetInvTransform() const
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

		return true;
	}
	return false;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



Transform2D::Transform2D()
{

}

Transform2D::~Transform2D()
{

}

void Transform2D::SetPos(const glm::vec2& pos)
{
	if (pos != o_position) {
		o_position = pos;
		is_invTransF_changed = is_TransF_changed = true;
	}
}

void Transform2D::SetScale(const glm::vec2& scale)
{
	if (scale != o_scale) {
		o_scale = scale;
		is_invTransF_changed = is_TransF_changed = true;
	}
}

void Transform2D::SetRot(float rot)
{
	if (rot != o_rot) {
		o_rot = rot;
		is_invTransF_changed = is_TransF_changed = true;
	}
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
	if (scale != glm::vec2(0,0) && scale != glm::vec2(1,1)) {
		SetScale(o_scale * scale);
	}
}

void Transform2D::Zoom(float scale)
{
	if (scale != 1 && scale != 0) {
		SetScale(o_scale * scale);
	}
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

bool Transform2D::ApplyTransform()
{
	if (is_TransF_changed) {

		LOOP(2) {
			o_Transform[i][i] = o_scale[i];
			o_Transform[2][i] = o_position[i] * o_scale[i];
		}
		o_Transform[2][2] = 1.0f;
		
		is_TransF_changed = false;
		is_invTransF_changed = true;
	}
	return is_TransF_changed;
}

bool Transform2D::ApplyAllTransform()
{
	return true;
}

bool Transform2D::GetInvTransform() const
{
	if (is_invTransF_changed) {
		LOOP(2) {
			o_InvTransform[i][i] = 1 / o_scale[i];
			o_InvTransform[2][i] = -o_position[i] / o_scale[i];
		}
		o_InvTransform[2][2] = 1.0f;
		
		is_invTransF_changed = false;
	}
	return is_invTransF_changed;
}
