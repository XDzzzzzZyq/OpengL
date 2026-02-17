#include "ShaderController.h"
#include "events/ShaderEvents.h"
#include "events/MaterialEvents.h"

void ShaderController::bind(EventPool& pool)
{
	// Shader Edit

	pool.subscribe<ShaderCodeCompileEvent>([this](ShaderCodeCompileEvent e) {
		e.shader->ParseShaderCode(e.code, e.type);
		e.shader->RelinkShader(e.type);
		});

	pool.subscribe<ShaderStructCompileEvent>([this](ShaderStructCompileEvent e) {
		e.shader->GenerateShader(e.type);
		e.shader->RelinkShader(e.type);
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
}
