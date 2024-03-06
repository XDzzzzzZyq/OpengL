#include "GameObject.h"
#include "macros.h"
#include "xdz_math.h"

int GameObject::count = 0;

int GameObject::GetObjectID() const
{
	return	id;
}

GameObject::GameObject()
{
	id = ++GameObject::count;

	const int R = id % 256;
	const int G = ((id - R) / 256) % 256;
	const int B = (id - R - G * 256) / 256 / 256;

	id_color = glm::vec3(R, G, B);
	id_color_rand = xdzm::rand3() / 2.0f + glm::vec3(0.5f);
	//id_color = id_color_rand;
}

GameObject::~GameObject()
{
	DEBUG(" >>[ " + o_name + " (ID=" + std::to_string(id) + ") ]");
}
