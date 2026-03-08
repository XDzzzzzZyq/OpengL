/**
 * @file ShaderIO.h
 * @brief File I/O for GLSL shader source files.
 *
 * ShaderIO provides stateless free functions for reading and writing GLSL shader
 * source code from/to the file system. It owns no GPU state and has no OpenGL
 * dependency, making it suitable for use in serialization, offline tools, or
 * unit tests that run without a GL context.
 *
 * Shaders::ReadShaderFile delegates here; callers may also use ShaderIO directly
 * to write modified or generated GLSL back to disk.
 *
 * Usage:
 * @code
 * std::string src = ShaderIO::Read("res/shaders/pps/FXAAAA.comp");
 * ShaderIO::Write("out/modified_shader.comp", src);
 * @endcode
 */

#pragma once

#include <string>

/**
 * @brief File I/O utilities for GLSL shader source files.
 *
 * All functions are stateless and operate purely on the file system.
 * They carry no OpenGL dependency and can be called before a GL context exists.
 */
namespace ShaderIO
{
	/**
	 * @brief Reads the entire contents of a text file into a string.
	 * @param path Full file system path to the shader source file.
	 * @return GLSL source as a string, or an empty string if the file cannot be opened.
	 */
	std::string Read(const std::string& path);

	/**
	 * @brief Writes GLSL source code to a file, overwriting any existing content.
	 * @param path Full file system path to write to.
	 * @param code GLSL source code to write.
	 * @note Creates or truncates the file at the given path.
	 */
	void Write(const std::string& path, const std::string& code);
}
