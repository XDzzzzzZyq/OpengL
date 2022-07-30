#pragma once
#include "glm/glm.hpp"

inline float sin01(const float& in) {
	return (glm::sin(in) + 1) / 2;

}

inline float map01_11(float inp) {
	return inp * 2 - 1;
}

inline float rand01() {
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline float rand11() {
	return map01_11(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}