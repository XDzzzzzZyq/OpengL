#include "GameObject.h"
#include "support.h"


int GameObject::count = 0;

int GameObject::GetObjectID() const
{
	return	id;
}

GameObject::GameObject()
{
	count++;
	id = count;

	int R = id % 256;
	int G = ((id - R) / 256) % 256;
	int B = (id - R - G * 256) / 256 / 256;

	id_color = glm::vec3(R, G, B);
	id_color_rand = glm::vec3(rand01(), rand01(), rand01());
	//id_color = id_color_rand;
}

GameObject::~GameObject()
{
	DEBUG(" >>[ " + o_name + " (ID=" + std::to_string(id) + ") ]")
}
