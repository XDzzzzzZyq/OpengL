#pragma once

#include <iostream>
#include <unordered_map>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>

#include "structs.h"
#include "xdz_math.h"
#include "operator.h"

#ifdef _DEBUG

#define DEBUG(x) std::cout<<(x)<<"\n";
#define DEBUGS(x) std::cout<__func__<<" : "<<__LINE__<<" : "<<(x)<<"\n";
#define GLDEBUG std::cout<<__func__<<" : "<<__LINE__<<" : "<<glDebug_xdz()<<"\n";

#else

#define DEBUG(x) 
#define DEBUGS(x) 
#define GLDEBUG 

#endif

//extern "C" {
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

inline std::string glDebug_xdz() {
	switch (glGetError())
	{
	case GL_NO_ERROR:
		return"GL_NO_ERROR";
			break;
	case GL_INVALID_ENUM:
		return"GL_INVALID_ENUM";
			break;
	case GL_INVALID_VALUE:
		return"GL_INVALID_VALUE";
			break;
	case GL_INVALID_OPERATION:
		return"INVALID_OPERATION";
			break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return"GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
	case GL_OUT_OF_MEMORY:
		return"GL_OUT_OF_MEMORY";
			break;
	}

}

inline float getrand() {

	return static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
}

inline Face Split(std::string_view in) {
	std::string out[3] = { "","","" };
	int count = 0;

	for (int i = 0; i < in.length(); i++)
	{
		if (in[i] == '/') {
			count++;
			continue;
		}
		else {
			out[count] += in[i];
		}
	}
	Face res;
	res.pos = 3 * std::stoi(out[0]) - 3;
	res.uv = 2 * std::stoi(out[1]) - 2;
	res.norm = 3 * std::stoi(out[2]) - 3;
	return res;
}

inline Reading ReadObj(const std::string& path, bool is_smooth = true) {
	Timer timer("Load OBJ");
	Reading result;

	std::vector<std::vector<int>> vertIndex;

	std::vector<float> tempdata[4];

	std::fstream obj;
	obj.open(path);
	std::string a = "";
	std::string last = "";

	Face face;
	bool is_face = true;

	int vert_count = 0;

	//timer.Tick();
	//Timer timer2;
	while (!obj.eof())
	{
		getline(obj, a);
		//if (last != a.substr(0, 2)) {
		last = a.substr(0, 2);
		std::istringstream str(a);
		if (last == "v ")
		{
			while (str >> last)
			{
				if (last == "v")continue;
				tempdata[0].emplace_back(atof(last.c_str()));
			}
			result.count[0]++;

		}
		else if (last == "vt")
		{
			while (str >> last)
			{
				if (last == "vt")continue;
				tempdata[1].emplace_back(atof(last.c_str()));

			}
			result.count[1]++;
		}
		else if (last == "vn")
		{
			while (str >> last)
			{
				if (last == "vn")continue;
				tempdata[2].emplace_back(atof(last.c_str()));

			}
			result.count[2]++;
		}
		else if (last == "f ")
		{
			if (is_face)
			{
				//init
				is_face = false;
				vertIndex.resize(result.count[0] * 4);
			}
			while (str >> last)
			{
				if (last == "f")continue;

				face.copy(Split(last));

				result.data_array.emplace_back(tempdata[0][face.pos + 0]);
				result.data_array.emplace_back(tempdata[0][face.pos + 1]);
				result.data_array.emplace_back(tempdata[0][face.pos + 2]);

				result.data_array.emplace_back(tempdata[1][face.uv + 0]);
				result.data_array.emplace_back(tempdata[1][face.uv + 1]);

				result.data_array.emplace_back(tempdata[2][face.norm + 0]);
				result.data_array.emplace_back(tempdata[2][face.norm + 1]);
				result.data_array.emplace_back(tempdata[2][face.norm + 2]);

				result.data_array.emplace_back(0.0f);
				result.data_array.emplace_back(0.0f);
				result.data_array.emplace_back(0.0f);

				vertIndex[(face.pos) / 3].push_back(result.count[3] * 3 + vert_count);

				vert_count++;
				if (vert_count >= 3)
					vert_count = 0;
			}
			result.count[3]++;
		}
		else if (last == "o ") {
			while (str >> last)
			{
				if (last == "o")continue;
				result.name += last.c_str();
			}
		}
	}


	//timer.Tick();
	//Timer timer3;
	if (is_smooth)
	{
		LOOP(vertIndex.size()) {

			if (vertIndex[i] == std::vector<int>{})
				break;

			float SMX = 0.0f, SMY = 0.0f, SMZ = 0.0f;
			for (int j = 0;j < vertIndex[i].size(); j++)
			{
				SMX += result.data_array[vertIndex[i][j] * 11 + 5] / vertIndex[i].size();
				SMY += result.data_array[vertIndex[i][j] * 11 + 6] / vertIndex[i].size();
				SMZ += result.data_array[vertIndex[i][j] * 11 + 7] / vertIndex[i].size();
			}
			for (int j = 0;j < vertIndex[i].size();j++)
			{
				result.data_array[vertIndex[i][j] * 11 + 8] = SMX;
				result.data_array[vertIndex[i][j] * 11 + 9] = SMY;
				result.data_array[vertIndex[i][j] * 11 + 10] = SMZ;

			}
		}

	}
	else {
		LOOP(vertIndex.size()) {

			if (vertIndex[i] == std::vector<int>{})
				break;
			for (int j = 0;j < vertIndex[i].size(); j++)
			{
				result.data_array[vertIndex[i][j] * 11 + 8 ] = result.data_array[vertIndex[i][j] * 11 + 5];
				result.data_array[vertIndex[i][j] * 11 + 9 ] = result.data_array[vertIndex[i][j] * 11 + 6];
				result.data_array[vertIndex[i][j] * 11 + 10] = result.data_array[vertIndex[i][j] * 11 + 7];
			}


		}
	}
	//timer.Tick();
	for (int i = 0;i < result.count[0];i++)
	{
		result.center[0] += tempdata[0][3 * i + 0] / (result.count[0] + 1);
		result.center[1] += tempdata[0][3 * i + 1] / (result.count[0] + 1);
		result.center[2] += tempdata[0][3 * i + 2] / (result.count[0] + 1);
	}

	return result;
}

inline glm::vec3 vecMult(const glm::vec3& a, const glm::vec3& b) {

	return glm::vec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

inline glm::vec3 vecDiv(const glm::vec3& a, const glm::vec3& b) {

	return glm::vec3(a[0] / b[0], a[1] / b[1], a[2] / b[2]);
}

inline glm::vec3 ImVec4_vec3(const ImVec4& color, float fac = 1.0f) {

	return fac * glm::vec3(color.x, color.y, color.z);
}

inline glm::vec3 ImVec4_vec3_Uni(const ImVec4& color, const float& fac) {

	return glm::vec3(color.x * fac - 0.5f * fac, color.y * fac - 0.5f * fac, color.z * fac - 0.5f * fac);
}

inline glm::vec3 stdVec3_vec3(const std::vector<float>& inp) {
	return glm::vec3{	inp[0], inp[1], inp[2]};
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