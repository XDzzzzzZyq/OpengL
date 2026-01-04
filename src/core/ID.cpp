#include "ID.h"
#include "macros.h"
#include "xdz_math.h"

int UID::count = 0;

UID::UID()
{
	id = ++UID::count;

}

ObjectID::ObjectID() {
	const int id = GetObjectID();
	const int R = id % 256;
	const int G = ((id - R) / 256) % 256;
	const int B = (id - R - G * 256) / 256 / 256;

	id_color = glm::vec3(R, G, B);
	id_color_rand = xdzm::rand3() / 2.0f + glm::vec3(0.5f);
}

ObjectID::~ObjectID()
{
	DEBUG(" >>[ " + o_name + " (ID=" + std::to_string(GetObjectID()) + ") ]");
}
