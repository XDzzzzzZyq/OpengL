#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Parameters.h"
#include "Texture.h"

#include "support.h"

#include <array>

enum MatParaType
{
	MAT_NONE = -1, MAT_ALBEDO, MAT_METAL, MAT_ROUGH, MAT_SPEC, MAT_EMIS_COL, MAT_EMIS_STR, MAT_ALPHA, MAT_NORMAL, MAT_BUMP, MAT_END
};

class Material
{
public:

	// data types for each materal parameter
	enum MatDataType
	{
		MPARA_FLT, MPARA_COL, MPARA_TEX, MPARA_CODE
	};

	using MatParamData = std::tuple<MatDataType, float, glm::vec3, TextureLib::TextureRes>;
	using MaterialRes = std::shared_ptr<Material>;
	static std::vector<std::string> mat_uniform_name;

public:

	std::string mat_name{ "Default" };
	std::unordered_map<MatParaType, MatParamData> mat_params;
	void InitParamData();

public:

	Material();
	~Material();

	static MaterialRes LoadMaterial(std::string _path = "");
	void ParseConfig(const std::string& _config);

public:

	bool is_mat_changed{ true };
	bool is_mat_struct_changed{ true };
	void SetMatParam(MatParaType _tar, float _var);
	void SetMatParam(MatParaType _tar, glm::vec3 _col);
	void SetMatParam(MatParaType _tar, TextureLib::TextureRes _tex);

	void BindMatTexture() const;

};

#endif