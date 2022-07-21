#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "Spirit.h"

#include "support.h"
#include "structs.h"
#include "Shaders.h"

#include "glm/glm.hpp"

enum EnvironmentType
{
	NONE_ENVIR, TEXTURE_ENVIR, COLOR_ENVIRN, NOISE_ENVIRN
};

class Environment : public GameObject, public Transform
{
	Texture envir_HDR;
	EnvironmentType envir_type = EnvironmentType::NONE_ENVIR;
	bool use_envir = false;

	float envir_power = 0.0f;
	glm::vec3 envir_color = glm::vec3(0.1f);

	Environment(const std::string& texpath, GLuint slot);
	Environment();
	~Environment();

	void ChangeEnvirTexture(const std::string& texpath) const;
	void ChangeEnvirType(const EnvironmentType& type) const;

	mutable std::vector<float> envir_floatData;
	void GenFloatData() const;


};

