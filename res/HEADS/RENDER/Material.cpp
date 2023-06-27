#include "Material.h"

std::vector<std::string> Material::mat_uniform_name = { 
	"m_albedo",
	"m_metal",
	"m_rough",
	"m_specu",
	"m_emis_c",
	"m_emis_s",
	"m_alpha",
	"m_normal",
	"m_bump"
};

void Material::InitParamData()
{
	mat_params = {
		{MAT_ALBEDO,	{MPARA_COL, 0.0, glm::vec3(1), nullptr}},
		{MAT_METAL,		{MPARA_FLT, 0.0, glm::vec3(0), nullptr}},
		{MAT_SPEC,		{MPARA_FLT, 0.5, glm::vec3(0), nullptr}},
		{MAT_ROUGH,		{MPARA_FLT, 0.5, glm::vec3(0), nullptr}},
		{MAT_EMIS_COL,	{MPARA_COL, 0.0, glm::vec3(0), nullptr}},
		{MAT_EMIS_STR,	{MPARA_FLT, 0.0, glm::vec3(0), nullptr}},
		{MAT_ALPHA,		{MPARA_FLT, 1.0, glm::vec3(0), nullptr}},
		{MAT_NORMAL,	{MPARA_COL, 0.0, glm::vec3(0), nullptr}},
		{MAT_BUMP,		{MPARA_FLT, 0.0, glm::vec3(0), nullptr}}
	};
}

Material::Material()
{
	InitParamData();
}

Material::~Material()
{

}

Material::MaterialRes Material::LoadMaterial(std::string _path /*= ""*/)
{
	return nullptr;
}

void Material::ParseConfig(const std::string& _config)
{

}

void Material::SetMatParam(MatParaType _tar, float _var)
{
	auto& [type, flt, _col, _tex] = mat_params[_tar];
	if (type != MPARA_FLT)
		is_mat_changed = is_mat_struct_changed = true;

	if (flt != _var)
		is_mat_changed = true;

	flt = _var;
	type = MPARA_FLT;
}

void Material::SetMatParam(MatParaType _tar, glm::vec3 _col)
{
	auto& [type, _flt, col, _tex] = mat_params[_tar];
	if (type != MPARA_COL)
		is_mat_changed = is_mat_struct_changed = true;

	if (col != _col)
		is_mat_changed = true;

	col = _col;
	type = MPARA_COL;
}

void Material::SetMatParam(MatParaType _tar, TextureLib::TextureRes _tex)
{
	auto& [type, _flt, _col, tex] = mat_params[_tar];
	if (type != MPARA_TEX)
		is_mat_changed = is_mat_struct_changed = true;

	if (tex != _tex)
		is_mat_changed = true;

	tex = _tex;
	type = MPARA_TEX;
}

void Material::BindMatTexture() const
{
	for (const auto& [ptype, pdata] : mat_params) {

		const auto& [dtype, _1, _2, tex] = pdata;
		if (dtype != MPARA_TEX) continue;

		tex->Bind(ptype);
	}
}
