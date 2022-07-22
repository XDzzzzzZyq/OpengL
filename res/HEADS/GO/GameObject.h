#pragma once
#include "structs.h"
#include "support.h"

class GameObject
{
private:
	int id;
public:
	std::string o_name;
	static int count;
	int GetObjectID() const;

	mutable bool is_rendered = true;
	GameObject();
	~GameObject();
	//glm::rotate(vec<3, T, Q> const& v, T const& angle, vec<3, T, Q> const& normal)
};
