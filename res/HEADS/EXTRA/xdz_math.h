#pragma once
#include "glm/glm.hpp"

inline float sin01(const float& in) {
	return (glm::sin(in) + 1) / 2;

}

inline float rand01() {
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
};