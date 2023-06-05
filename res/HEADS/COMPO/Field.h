#pragma once

#include "support.h"
#include "Transform.h"

enum FieldType
{
	NONE_FIELD, LINEAR_FIELD, SPHERE_FIELD, RADICAL_FIELD, RANDOM_FIRLD, BOX_FIELD
};

class Field : public Transform3D
{
public:
	FieldType fld_type = NONE_FIELD;
};

