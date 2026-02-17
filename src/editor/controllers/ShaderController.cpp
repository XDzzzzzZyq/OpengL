#include "ShaderController.h"
#include "events/ShaderEvents.h"

void ShaderController::bind(EventPool& pool)
{
	pool.subscribe<ShaderCodeCompileEvent>([this](ShaderCodeCompileEvent e) {
		e.shader->ParseShaderCode(e.code, e.type);
		e.shader->RelinkShader(e.type);
		});

	pool.subscribe<ShaderStructCompileEvent>([this](ShaderStructCompileEvent e) {
		e.shader->GenerateShader(e.type);
		e.shader->RelinkShader(e.type);
		});
}
