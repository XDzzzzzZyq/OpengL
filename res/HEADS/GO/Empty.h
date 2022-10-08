#pragma once
#include "Transform.h"
#include "GameObject.h"

enum EmptyType
{
	BLANK_EMPTY, POST_PROC_EMPTY, FIELD_EMPTY
};

class Empty : public GameObject, public Transform3D
{
private:
	EmptyType empty_type;
public:
	Empty(EmptyType type);
	~Empty();
};

