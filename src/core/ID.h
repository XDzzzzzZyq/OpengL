#pragma once

#include <string>

class UID {
private:
	int id;
	static int count;

public:
	UID();
	virtual ~UID() = default;

	inline int GetObjectID() const { return id; };
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
