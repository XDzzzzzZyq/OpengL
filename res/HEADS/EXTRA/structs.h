#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "ImGui/imgui.h"

#include <iostream>
#include <unordered_map>
#include <ctime>
#include <vector>

#define SCREEN_W 1740
#define SCREEN_H 810
#define LOOP(x) for(int i = 0; i < x; i++)
#define LOOP_N(x, n) for(int n = 0; n < x; n++)



struct ShaderPair {
	std::string verShader;
	std::string fragShader;

};

struct Timer
{
	time_t start = 0, end = 0, tick = 0, temp = 0;
	time_t __duration = 0;
	int fact = 1;
	std::string name;
	bool is_print;
	Timer(std::string name="Thread", int fact = 1, bool print = true) 
		:name(name), fact(fact), is_print(print)
	{
#ifdef _DEBUG
		if (is_print)
			std::cout << "[ " + name + " Start ]\n";
#endif // _DEBUG
		start = tick = temp = clock();
	}

	time_t Tick() {
		/*std::cout << 000000 << "\n";*/
		tick = clock();
		__duration = tick - temp;
		temp = tick;
#ifdef _DEBUG
		if (is_print)
			std::cout /*<< "\r"*/ << "\_[ duration = " << __duration * fact << "ms ]\n";
#endif // _DEBUG
		return __duration;
	}

	time_t GetDuration() {
		end = clock();
#ifdef _DEBUG
		if (is_print)
			std::cout /*<< "\r"*/ << "[ " + name + " Whole Time = " << (end - start) * fact << "ms ]\n";
#endif // _DEBUG
		return (end - start);
	}

	~Timer() {
		end = clock();
		__duration = end - start;
#ifdef _DEBUG
		if (is_print)
			std::cout /*<< "\r"*/ << "[ "+name+" Whole Time = " << __duration * fact << "ms ]\n";
#endif // _DEBUG
	}
};

struct Reading
{
	std::vector<float> data_array;
	std::vector<float> center = { 0,0,0 };
	int count[4] = { 0,0,0,0 };

	std::string name = "";
};

struct Face
{
	int pos, uv, norm, smo_norm;
	void copy(const Face& inp) {
		pos = inp.pos;
		uv = inp.uv;
		norm = inp.norm;
		smo_norm = inp.smo_norm;
	}
	void print() {
		std::cout << pos << " " << uv << " " << norm << "\n";
	}
};

template<int _L>
struct AverageTime
{
	float result = 0;
	void Update(float _rate) {
		result += (_rate - result) / _L;
	}
};

struct OutlineElement
{
	int TYPE;
	int ID;
	std::string NAME;
	int LEVEL;

	OutlineElement(int a, int id, const std::string& b, int c) 
		:TYPE(a), ID(id), NAME(b), LEVEL(c)
	{}
};
//			| TYPE | ID | NAME | LEVEL |
typedef std::vector<OutlineElement> OutlineData;