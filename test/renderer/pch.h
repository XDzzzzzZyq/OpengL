//
// pch.h
//

#pragma once

#include "gtest/gtest.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

inline std::string error_info(GLuint err) {
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

#define GLERRTEST do{ const GLuint err = glGetError(); EXPECT_EQ(err, GL_NO_ERROR) << error_info(err); }while(0)

class RendererEnvir : public testing::Test {
protected:

	static void SetUpTestSuite();
	static void TearDownTestSuite();
	virtual void SetUp() override;
	virtual void TearDown() override;

public:

	std::string tex_root = SOL_DIR"res/tex/";
};