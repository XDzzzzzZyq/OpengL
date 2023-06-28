#pragma once
#include "structs.h"
#include "support.h"

enum GOType
{
	NONE_GO = -1, GO_CAM, GO_MESH, GO_LIGHT, GO_POLYLIGHT, GO_ENVIR, GO_SPRITE, GO_DL, GO_DP, GO_DM
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

	virtual void* GetShaderStruct() { DEBUG("there is no shader be found")		return nullptr; }
	virtual void* GetTransform()	{ DEBUG("there is no transform be found")	return nullptr; }
	virtual void* GetMaterial()		{ DEBUG("there is no material be found")	return nullptr; }

	GameObject();
	~GameObject();
};
