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

	glm::vec3 a(2,2,2);
	std::vector<glm::vec3> list = { {1,1,1},{-1,-1,-1} };

	glm::mat4 trans(1);	DEBUG(trans)
	trans = glm::translate(trans, glm::vec3(2, 0, 0));	DEBUG(trans)
	trans = glm::scale(trans, glm::vec3(2, 2, 2));	DEBUG(trans)

	for (const auto& i : list) {
		DEBUG(trans*i)
	}
	
}