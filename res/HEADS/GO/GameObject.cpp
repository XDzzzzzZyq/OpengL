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

}

GameObject::~GameObject()
{
	DEBUG(" >>[ "+o_name + " ]")
}
