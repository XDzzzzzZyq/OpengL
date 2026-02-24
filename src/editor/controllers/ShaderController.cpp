#include "ShaderController.h"
#include "events/ShaderEvents.h"
#include "events/MaterialEvents.h"

#include "Shaders.h"

#include <algorithm>
void UpdateMaterial(Shaders* shader, Material* mat)
{
	Shaders::ShaderUnit* active_unit = shader->GetShaderUnit(FRAGMENT_SHADER);
	if (!active_unit->sh_struct.has_value())
		return;

	ShaderStruct& sh_struct = active_unit->sh_struct.value();
	for (const auto& [ptype, pdata] : mat->mat_params) {
		const auto& [dtype, dfloat, dcol, dtex] = pdata;

		if (ptype == Material::MAT_NORMAL || ptype == Material::MAT_BUMP)
			continue;  // TODO: implement in the future

		auto loc_const = std::find_if(
			sh_struct.const_list.begin(),
			sh_struct.const_list.end(),
			[ptype](const S_const& con) ->bool {
				return std::get<1>(con) == "m_" + Material::mat_uniform_name[ptype];     // e.g. Material::MAT_ALBEDO -> m_albedo
			}
		);

		auto loc_uniform = std::find_if(
			sh_struct.uniform_list.begin(),
			sh_struct.uniform_list.end(),
			[ptype](const S_U& uni) ->bool {
				return std::get<0>(uni) == "U_" + Material::mat_uniform_name[ptype];     // e.g. Material::MAT_ALBEDO -> m_albedo
			}
		);

		switch (dtype)
		{
		case Material::MPARA_FLT:

			if (loc_uniform != sh_struct.uniform_list.end())
				std::get<1>(*loc_uniform) = FLOAT_PARA;
			else
				sh_struct.SetUni(FLOAT_PARA, 1, "U_" + Material::mat_uniform_name[ptype]);

			if (loc_const != sh_struct.const_list.end())
				*loc_const = ShaderLib::v_material[ptype];
			else
				sh_struct.const_list.emplace_back(ShaderLib::v_material[ptype]);

			break;
		case Material::MPARA_COL:

			if (loc_uniform != sh_struct.uniform_list.end())
				std::get<1>(*loc_uniform) = VEC3_PARA;
			else
				sh_struct.SetUni(VEC3_PARA, 1, "U_" + Material::mat_uniform_name[ptype]);

			if (loc_const != sh_struct.const_list.end())
				*loc_const = ShaderLib::v_material[ptype];
			else
				sh_struct.const_list.emplace_back(ShaderLib::v_material[ptype]);

			break;
		case Material::MPARA_TEX:

			if (loc_uniform != sh_struct.uniform_list.end())
				std::get<1>(*loc_uniform) = TEXTURE_PARA;
			else
				sh_struct.SetUni(TEXTURE_PARA, 1, "U_" + Material::mat_uniform_name[ptype]);

			if (loc_const != sh_struct.const_list.end())
				*loc_const = ShaderLib::t_material[ptype];
			else
				sh_struct.const_list.emplace_back(ShaderLib::t_material[ptype]);

			break;
		}
	}

	sh_struct.is_struct_changed = true;
	shader->is_shader_changed = true;

	shader->GenerateShader(FRAGMENT_SHADER);
	shader->RelinkShader(FRAGMENT_SHADER);
}

void ShaderController::bind(EventPool& pool)
{
	// Shader Edit

	pool.subscribe<ShaderCodeCompileEvent>([this, &pool](ShaderCodeCompileEvent e) {
		e.shader->ParseShaderCode(e.code, e.type);
		e.shader->RelinkShader(e.type);

		pool.emit<ShaderChangedEvent>({ e.shader, e.type });
		});

	pool.subscribe<ShaderStructCompileEvent>([this, &pool](ShaderStructCompileEvent e) {
		e.shader->GenerateShader(e.type);
		e.shader->RelinkShader(e.type);

		pool.emit<ShaderChangedEvent>({ e.shader, e.type });
		});

	pool.subscribe<ShaderStructAddArrayBufferEvent>([this](ShaderStructAddArrayBufferEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		const auto& [loc, name, type] = e.layout;
		unit->sh_struct->SetAB(loc, type, name);
		});

	pool.subscribe<ShaderStructAddPassEvent>([this](ShaderStructAddPassEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		const auto& [loc, name, type] = e.layout;
		unit->sh_struct->SetPass(loc, type, name);
		});

	pool.subscribe<ShaderStructAddInputEvent>([this](ShaderStructAddInputEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		const auto& [name, type, count] = e.input;
		unit->sh_struct->SetInp(type, count, name);
		});

	pool.subscribe<ShaderStructAddOutputEvent>([this](ShaderStructAddOutputEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		const auto& [name, type, count] = e.output;
		unit->sh_struct->SetOut(type, count, name);
		});

	pool.subscribe<ShaderStructAddUniformEvent>([this](ShaderStructAddUniformEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		const auto& [name, type, count] = e.uniform;
		unit->sh_struct->SetUni(type, count, name);
		});

	pool.subscribe<ShaderStructAddStructEvent>([this](ShaderStructAddStructEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		unit->sh_struct->DefStruct(e.name, e.args);
		});

	pool.subscribe<ShaderStructAddGlobEvent>([this](ShaderStructAddGlobEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		const auto& [name, type, val] = e.glob;
		unit->sh_struct->SetGlob(type, val, name);
		});

	pool.subscribe<ShaderStructAddConstEvent>([this](ShaderStructAddConstEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		const auto& [type, name, content, _] = e.con;
		unit->sh_struct->SetConst(type, name, content);
		});

	pool.subscribe<ShaderStructAddFuncEvent>([this](ShaderStructAddFuncEvent e) {
		auto* unit = e.shader->GetShaderUnit(e.type);
		if (!unit || !unit->sh_struct.has_value()) return;
		const auto& [type, name, content, args] = e.func;
		unit->sh_struct->DefFunc(type, name, content, args);
		});

	// Material Edit

	pool.subscribe<MaterialNameChangedEvent>([this](MaterialNameChangedEvent e) {
		if (!e.material)
			return;

		if (e.material->mat_name != e.name) {
			e.material->mat_name = e.name;
			e.material->is_mat_changed = true;
		}
		});

	pool.subscribe<MaterialFloatChangedEvent>([this](MaterialFloatChangedEvent e) {
		if (!e.material)
			return;
		if (e.data_type == Material::MPARA_FLT)
			e.material->SetMatParam(e.param_type, e.value);
		});

	pool.subscribe<MaterialColorChangedEvent>([this](MaterialColorChangedEvent e) {
		if (!e.material)
			return;
		if (e.data_type == Material::MPARA_COL)
			e.material->SetMatParam(e.param_type, e.color);
		});

	pool.subscribe<MaterialTextureNameChangedEvent>([this](MaterialTextureNameChangedEvent e) {
		if (!e.material)
			return;
		});

	pool.subscribe<MaterialTypeChangedEvent>([this, &pool](MaterialTypeChangedEvent e) {
		if (!e.material)
			return;
		e.material->SetMatParam(e.param_type, e.data_type);
		pool.emit<MaterialStructChangedEvent>({e.obj, e.material});
		});

	pool.subscribe<MaterialStructChangedEvent>([this, &pool ](MaterialStructChangedEvent e) {
		if (!e.material)
			return;
		UpdateMaterial((Shaders*)(e.obj->GetShader()), e.material);

		pool.emit<ShaderChangedEvent>({ (Shaders*)(e.obj->GetShader()), FRAGMENT_SHADER });
		});
}
