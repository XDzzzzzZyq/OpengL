#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <ctime>
#include <iostream>

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

inline std::string glDebug_xdz() {
	const GLuint err = glGetError();
	switch (err)
	{
#define GL_ERR(err) case GL_##err: return #err
		GL_ERR(NO_ERROR);
		GL_ERR(INVALID_ENUM);
		GL_ERR(INVALID_VALUE);
		GL_ERR(INVALID_OPERATION);
		GL_ERR(INVALID_FRAMEBUFFER_OPERATION);
		GL_ERR(OUT_OF_MEMORY);
	default:
		return "UNKOWN_ERROR" + std::to_string(err);
	}
}
