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

	mutable bool is_viewport = true;
	mutable bool is_rendered = true;
	void SetVisible(bool v, bool r) {
		is_viewport = v;
		is_rendered = r;
	}

	mutable bool is_selected = false;
	mutable bool is_activated = false;

	GameObject();
	~GameObject();
	//glm::rotate(vec<3, T, Q> const& v, T const& angle, vec<3, T, Q> const& normal)
};
