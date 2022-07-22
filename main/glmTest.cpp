#include "support.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>

#define DEBUG(x) std::cout<<x<<"\n";

using std::cout;
using std::endl;
using std::cin;

int main() {

	glm::vec3 a(30.0f, 20.0f, 10.0f);
	glm::mat4 mat = glm::mat4_cast(glm::qua<float>(glm::radians(a)));
	DEBUG(mat);
	cout << mat[0][2] << endl;
	
	cin.get();
}