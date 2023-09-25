#pragma once

#include "structs.h"
#include "glm/glm.hpp"

enum GOType
{
	NONE_GO = -1, GO_CAM, GO_MESH, GO_LIGHT, GO_POLYLIGHT, GO_ENVIR, GO_SPRITE, GO_DL, GO_DP, GO_DM, GO_SDFFIELD
};

class GameObject
{
private:
	int id;
public:
	mutable glm::vec3 id_color;
	mutable glm::vec3 id_color_rand;

	std::string o_name;
	static int count;
	int GetObjectID() const;

	GOType o_type = NONE_GO;

	mutable bool is_viewport = true;
	mutable bool is_rendered = true;
	void SetVisible(bool v, bool r) {
		is_viewport = v;
		is_rendered = r;
	}

	mutable bool is_selected = false;
	mutable bool is_activated = false;

	virtual void* GetShaderStruct() { return nullptr; }
	virtual void* GetTransform()	{ return nullptr; }
	virtual void* GetMaterial()		{ return nullptr; }

	GameObject();
	~GameObject();
};
