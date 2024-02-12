#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "macros.h"
#include "operator.h"

namespace xdzm {
	inline glm::mat4 lookAt(const glm::vec3 pos, const glm::vec3 tar, const glm::vec3 up) {
		glm::vec3 const forward(glm::normalize(tar - pos));
		glm::vec3 const rightwd(glm::normalize(glm::cross(forward, up)));
		glm::vec3 const upward(glm::cross(rightwd, forward));

		glm::mat4 Result(1);

		LOOP(3)
			Result[0][i] = rightwd[i];
		LOOP(3)
			Result[1][i] = upward[i];
		LOOP(3)
			Result[2][i] = -forward[i];
		//Result[3][0] = -glm::dot(s, pos);
		//Result[3][1] = -glm::dot(u, pos);
		//Result[3][2] = glm::dot(f, pos);
		return Result;
	}

	inline glm::vec3 Vec3Spin(const glm::vec3& vecInp, const glm::vec3& anch, const glm::vec3& axis, const float& angle)
	{
		glm::vec3 result = vecInp;
		glm::qua rotQua = glm::qua<float>(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
		rotQua = glm::rotate(rotQua, angle, axis);
		glm::mat4 rotMat = glm::mat4_cast(rotQua);

		result -= anch;
		return anch + rotMat * result;
	}

	inline glm::mat4 transpose(const glm::mat4 _tar) {
		glm::mat4 result(glm::transpose(glm::mat3(_tar)));
		result[3][3] = 1;
		result = glm::translate(result, glm::vec3(_tar[3]));
		return result;
	}

	static const glm::mat4 identityMatrix =
	{ 1.f, 0.f, 0.f, 0.f,
	  0.f, 1.f, 0.f, 0.f,
	  0.f, 0.f, 1.f, 0.f,
	  0.f, 0.f, 0.f, 1.f };

	template<size_t S, typename T>
	static float _matrix_sum(const T _m) {
		float res = 0;
		LOOP(S)
			LOOP_N(S, j)
			res += _m[i][j];
		return res;
	}

	static float matrix_diff(const glm::mat4 _m1, const glm::mat4 _m2) {
		return abs(_matrix_sum<3>(glm::mat3(_m2) - glm::mat3(_m1)));
	}
}
