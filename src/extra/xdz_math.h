#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "macros.h"
#include "operator.h"

#include <random>

namespace xdzm {
	inline float sin01(float in) {
		return (glm::sin(in) + 1) / 2;
	}

	inline float map01_11(float inp) {
		return inp * 2 - 1;
	}

	inline float map(float inp, float i_min, float i_max, float o_min, float o_max) {
		float re = (inp - i_min) / (i_max - i_min);
		return re * (o_max - o_min) + o_min;
	}

	template<float i_min, float i_max, float o_min, float o_max>
	inline float map(float inp) {
		float re = (inp - i_min) / (i_max - i_min);
		return re * (o_max - o_min) + o_min;
	}

	inline float rand01() {
		std::mt19937 rng(std::random_device{}());
		return std::uniform_real_distribution<float>{0.f, 1.f}(rng);
	}

	inline float rand11() {
		return map01_11(rand01());
	}

	inline glm::vec2 rand2() {
		return { rand11(), rand11() };
	}

	inline glm::vec3 rand3() {
		return { rand11(), rand11(), rand11() };
	}

	inline glm::vec3 rand3n(float r = 1.0f) {
		return r * glm::normalize(rand3());
	}

	inline glm::vec3 rand3nh(float r = 1.0f) {
		glm::vec3 _rand = rand3();
		_rand.z = _rand.z / 2.0f + 0.5f;
		return r * glm::normalize(_rand);
	}

	inline std::vector<glm::vec3> rand3nv(size_t s) {
		std::vector<glm::vec3> res(s);
		LOOP(s) res[i] = rand3n();
		return res;
	}

	inline std::vector<glm::vec3> rand3nhv(size_t s) {
		std::vector<glm::vec3> res(s);
		LOOP(s) res[i] = rand3nh();
		return res;
	}

	inline std::vector<glm::vec3> rand3hKernel(size_t s) {
		std::vector<glm::vec3> res(s);
		const float r = rand01()*0.7 + 0.3;
		LOOP(s) res[i] = r*r*rand3nh();
		return res;
	}

	inline glm::vec4 rand4() {
		return { rand11(), rand11(), rand11(), rand11() };
	}

	inline glm::vec3 ImVec4_vec3(const ImVec4& color, float fac = 1.0f) {

		return fac * glm::vec3(color.x, color.y, color.z);
	}

	inline glm::vec3 ImVec4_vec3_Uni(const ImVec4& color, const float& fac) {

		return glm::vec3(color.x * fac - 0.5f * fac, color.y * fac - 0.5f * fac, color.z * fac - 0.5f * fac);
	}

	inline glm::vec3 stdVec3_vec3(const std::vector<float>& inp) {
		return glm::vec3{ inp[0], inp[1], inp[2] };
	}

	inline std::vector<float> vec3_stdVec3(const glm::vec3& inp) {
		return std::vector<float>{	inp[0], inp[1], inp[2]};
	}

	inline std::vector<float> vec3_stdVec6(const glm::vec3& inp, const glm::vec3& inp2) {

		return std::vector<float>{inp[0], inp[1], inp[2], inp2[0], inp2[1], inp2[2]};
	}

	inline float float_dist(const float& a, const float& b) {
		return glm::sqrt(a * a + b * b);
	}

	inline float dir_float_dist(const float& a, const float& b) {
		return b > a ? -float_dist(a, b) : float_dist(a, b);
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