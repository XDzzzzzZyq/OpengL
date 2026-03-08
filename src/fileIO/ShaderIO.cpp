#include "ShaderIO.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ShaderIO
{

std::string Read(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "ShaderIO::Read failed to open: " << path << "\n";
		return {};
	}
	std::stringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

void Write(const std::string& path, const std::string& code)
{
	std::ofstream file(path, std::ios::out | std::ios::trunc);
	if (!file.is_open())
	{
		std::cout << "ShaderIO::Write failed to open: " << path << "\n";
		return;
	}
	file << code;
	if (!file.good())
		std::cout << "ShaderIO::Write error writing: " << path << "\n";
}

} // namespace ShaderIO
