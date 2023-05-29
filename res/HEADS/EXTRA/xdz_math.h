#pragma once
#include "glm/glm.hpp"

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
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
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
}