#include "support.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>

#define DEBUG(x) std::cout<<x<<"\n";

using std::cout;
using std::endl;
using std::cin;

glm::vec3 genDirFromAngle(glm::vec2 angle) {
	return glm::vec3(cos(angle.y) * cos(angle.x), cos(angle.y) * sin(angle.x), sin(angle.y));
}

constexpr float pi = 3.1415926;
constexpr int max_step = 2;

#if 0
int main() {
	glm::vec2 angle = { 0,0 };

	glm::vec3 dir_f = genDirFromAngle(angle);
	glm::vec3 dir_u = genDirFromAngle(angle + glm::vec2(0, pi / 2));
	glm::vec3 dir_l = glm::cross(dir_f, dir_u);

// 	DEBUG(dir_f)
// 		DEBUG(dir_u)
// 		DEBUG(dir_l)
	for (float i = -max_step; i <= max_step; i++) {
		int j_m = max_step - abs(i);
		DEBUG("_______")
		for (float j = -j_m; j <= j_m; j++) {
			float a = i / max_step;
			float b = j / max_step;
			float c = 1 - abs(a) - abs(b);

			glm::vec3 dir = glm::normalize(a * dir_u + b * dir_l + c * dir_u);
			DEBUG(dir)
		}

	}
}
#endif
