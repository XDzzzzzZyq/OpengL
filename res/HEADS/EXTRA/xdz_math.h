#pragma once
#include "glm/glm.hpp"

inline float sin01(const float& in) {
	return (glm::sin(in) + 1) / 2;

}