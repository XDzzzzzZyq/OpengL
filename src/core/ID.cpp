#include "ID.h"
#include "macros.h"
#include "xdz_math.h"

int UID::count = 0;

UID::UID()
{
	id = ++UID::count;

}

ObjectID::ObjectID() {}

ObjectID::~ObjectID()
{
	DEBUG(" >>[ " + o_name + " (ID=" + std::to_string(GetObjectID()) + ") ]");
}
