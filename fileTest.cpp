#include<iostream>
#include<fstream>
#include<string>
#include <sstream>std::stringstream shaders;

using std::string;
using std::cout;
using std::endl;

struct ShaderPair {
	string verShader;
	string fragShader;

};

static ShaderPair ParseShader(const string& path) {
	std::ifstream stream(path);

	string line;

	int row = 0;

	enum class LineType
	{
		N = -1, V, F
	};
	LineType mode = LineType::N;

	std::stringstream shaders[2];

	while (getline(stream, line)) {
		if (line.find("shader") != string::npos) {
			if (line.find("vertex") != string::npos) {
				mode = LineType::V;
				cout << "vertex" << endl;
			}
			else if (line.find("fragment") != string::npos) {
				mode = LineType::F;
				cout << "frag" << endl;
			}

		}
		else {
			shaders[(int)mode] << line << "\n";
			cout << row << endl;
		}
		row++;
	}

	return { shaders[0].str(),shaders[1].str() };
}

int main() {
	ShaderPair shaders = ParseShader("res/shaders/test.shader");
	cout << shaders.verShader << endl;
	cout << shaders.fragShader << endl;

	return 0;
}