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

	template<typename _Ele>
	inline _Ele map01_11(_Ele inp) {
		return inp * _Ele(2) - _Ele(1);
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
}