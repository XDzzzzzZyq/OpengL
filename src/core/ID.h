#pragma once

#include "glm/glm.hpp"
#include <string>

class UID {
private:
	int id;
	static int count;

public:
	UID();
	virtual ~UID() = default;

	int GetObjectID() const { return id; };
	static int GetTotalAllocated() { return count; };
};

class ObjectID : public UID
{
public:
	enum GOType
	{
		NONE_GO = -1, GO_CAM, GO_MESH, GO_LIGHT, GO_POLYLIGHT, GO_ENVIR, GO_SPRITE, GO_DL, GO_DP, GO_DM, GO_SDFFIELD
	};

public:
	// TODO: remove it from here
	mutable glm::vec3 id_color;
	mutable glm::vec3 id_color_rand;

	std::string o_name;

	GOType o_type = NONE_GO;

	mutable bool is_viewport = true;
	mutable bool is_rendered = true;
	void SetVisible(bool v, bool r) {
		is_viewport = v;
		is_rendered = r;
	}

	virtual void* GetShader()		{ return nullptr; }
	virtual void* GetTransform()	{ return nullptr; }
	virtual void* GetMaterial()		{ return nullptr; }

	ObjectID();
	~ObjectID();
};
