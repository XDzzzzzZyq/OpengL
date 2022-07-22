#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "ImGui/imgui.h"
inline std::ostream& operator<<(std::ostream& stream, const glm::vec4& vec)
{
	stream << "[ " << vec[0] << " , " << vec[1] << " , " << vec[2] << " , " << vec[3] << " ]\n";
	return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const glm::vec3& vec)
{
	stream << "[ " << vec[0] << " , " << vec[1] << " , " << vec[2] << " ]\n";
	return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const glm::mat4& vec)
{
	stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
	stream.setf(std::ios_base::showpoint);
	stream.precision(2);


	stream << "\n[ " << vec[0][0] << " , " << vec[1][0] << " , " << vec[2][0] << " , " << vec[3][0] << " ]\n";
	stream <<   "| " << vec[0][1] << " , " << vec[1][1] << " , " << vec[2][1] << " , " << vec[3][1] << " |\n";
	stream <<   "| " << vec[0][2] << " , " << vec[1][2] << " , " << vec[2][2] << " , " << vec[3][2] << " |\n";
	stream <<   "[ " << vec[0][3] << " , " << vec[1][3] << " , " << vec[2][3] << " , " << vec[3][3] << " ]\n";
	return stream;
}

inline glm::vec3 operator*(const glm::mat4& mat, const glm::vec3 vec) {
	return glm::vec3(
		mat[0][0] * vec[0] + mat[1][0] * vec[1] + mat[2][0] * vec[2],
		mat[0][1] * vec[0] + mat[1][1] * vec[1] + mat[2][1] * vec[2],
		mat[0][2] * vec[0] + mat[1][2] * vec[1] + mat[2][2] * vec[2]
	);
}

inline glm::vec3 operator*=(const glm::vec3 vec, const glm::mat4& mat) {
	return mat * vec;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec) {

	stream << "[";
	for (const T& ele : vec)
	{
		stream << ele << ",";
	}
	stream << "]\n";
	return stream;
}

inline std::string operator+(const std::string& content, int number) {
	std::string temp = "";
	char t = 0;
	while (true) {
		t = number % 10 + '0';
		temp = t + temp;
		number /= 10;
		if (number == 0) {
			return content + temp;
		}
	}
}

inline std::string operator+=(std::string content, int number) {
	return content = content + number;
}