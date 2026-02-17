#pragma once
#include "Material.h"

#include <string>

struct MaterialNameChangedEvent {
	Material* material;
	std::string name{};
};

struct MaterialFloatChangedEvent {
	Material* material;
	MatParaType param_type{ MAT_NONE };
	Material::MatDataType data_type{ Material::MPARA_FLT };
	float value{ 0.0f };
};

struct MaterialColorChangedEvent {
	Material* material;
	MatParaType param_type{ MAT_NONE };
	Material::MatDataType data_type{ Material::MPARA_COL };
	glm::vec3 color{ 0.0f };
};

struct MaterialTextureNameChangedEvent {
	Material* material;
	MatParaType param_type{ MAT_NONE };
	Material::MatDataType data_type{ Material::MPARA_TEX };
	std::string name{};
};
