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

inline std::ostream& operator<<(std::ostream& stream, const ImVec2& vec)
{
	stream << "[ " << vec[0] << " , " << vec[1] << " ]\n";
	return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const glm::vec2& vec)
{
	stream << "[ " << vec[0] << " , " << vec[1] << " ]\n";
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

inline std::ostream& operator<<(std::ostream& stream, const glm::mat3& vec)
{
	stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
	stream.setf(std::ios_base::showpoint);
	stream.precision(2);


	stream << "\n[ " << vec[0][0] << " , " << vec[1][0] << " , " << vec[2][0] << " ]\n";
	stream << "	 | " << vec[0][1] << " , " << vec[1][1] << " , " << vec[2][1] << " |\n";
	stream << "	 [ " << vec[0][2] << " , " << vec[1][2] << " , " << vec[2][2] << " ]\n";
	return stream;
}

inline glm::vec3 operator*(const glm::mat4& mat, const glm::vec3 vec) {
	return {
		mat[0][0] * vec[0] + mat[1][0] * vec[1] + mat[2][0] * vec[2] + mat[3][0],
		mat[0][1] * vec[0] + mat[1][1] * vec[1] + mat[2][1] * vec[2] + mat[3][1],
		mat[0][2] * vec[0] + mat[1][2] * vec[1] + mat[2][2] * vec[2] + mat[3][2]
	};
}

inline glm::vec2 operator*(const glm::mat3& mat, const glm::vec2 vec) {
	return {
		mat[0][0] * vec[0] + mat[1][0] * vec[1] + mat[2][0],
		mat[0][1] * vec[0] + mat[1][1] * vec[1] + mat[2][1]
	};
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

template <typename T>
inline std::vector<T> operator+(const std::vector<T>& base, const glm::vec3& inp) {

}

inline std::string operator+=(const std::string& bas, const std::string& add) {
	return bas + add;
}

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
	return { a[0] + b[0], a[1] + b[1] };
}

inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
	
	return { a[0] - b[0], a[1] - b[1] };
}

inline ImVec2 operator*(const ImVec2& a, const ImVec2& b) {
	return 	{ a[0] * b[0], a[1] * b[1] };
}

inline ImVec2 operator/(const ImVec2& a, const ImVec2& b) {
	return { a[0] / b[0], a[1] / b[1] };
}

inline bool operator==(const ImVec2& a, const ImVec2& b) {
	return a[0]==b[0]&&a[1]==b[1];
}

inline bool operator!=(const ImVec2& a, const ImVec2& b) {
	return a[0] != b[0] || a[1] != b[1];
}

inline bool operator<(const ImVec2& a, const ImVec2& b) {
	return a[0] < b[0] && a[1] < b[1];
}

inline bool operator>(const ImVec2& a, const ImVec2& b) {
	return a[0] > b[0]&& a[1] > b[1];
}

inline ImVec2 operator/(const ImVec2& a, float fac) {
	return { a[0] / fac, a[1] / fac };
}

inline ImVec2 operator+(const ImVec2& a, const glm::vec2& b) {
	return { a[0] + b[0], a[1] + b[1] };
}

inline ImVec2 operator-(const ImVec2& a, const glm::vec2& b) {
	return { a[0] - b[0], a[1] - b[1] };
}


inline ImVec2 operator*(const ImVec2& a, const glm::vec2& b) {
	return { a[0] * b[0], a[1] * b[1] };
}

inline glm::vec2 operator+(const glm::vec2& a, const ImVec2& b) {
	return { a[0] + b[0], a[1] + b[1] };
}

inline glm::vec2 operator-(const glm::vec2& a, const ImVec2& b) {
	return { a[0] - b[0], a[1] - b[1] };
}

inline glm::vec2 operator*(const glm::vec2& a, const ImVec2& b) {
	return { a[0] * b[0], a[1] * b[1] };
}