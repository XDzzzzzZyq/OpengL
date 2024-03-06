#pragma once

#ifdef _DEBUG

#include <GL/glew.h>
#include <iostream>
#include <string>

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

#define DEBUG(x) std::cout<<(x)<<"\n"
#define DEBUGS(x) std::cout<__func__<<" : "<<__LINE__<<" : "<<(x)<<"\n"
#define GLDEBUG std::cout<<__func__<<" : "<<__LINE__<<" : "<<glDebug_xdz()<<"\n"

#else

#define DEBUG(x)
#define DEBUGS(x)
#define GLDEBUG

#endif


#define SCREEN_W 1740
#define SCREEN_H 810
#define LOOP_N(x, n) for(int n = 0; n < (int)x; n++)
#define LOOP(x) LOOP_N(x, i)

#define OPTIONS(...) std::vector<std::string>{__VA_ARGS__}