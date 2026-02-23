#include "Light.h"
#include "xdz_math.h"
#include "Input.h"
#include "shaders/ComputeShader.h"

float Light::sun_shaodow_field = 5.0f;
float Light::sun_shaodow_near = -5.0f;
float Light::sun_shaodow_far = 5.0f;

float Light::point_shaodow_near = 0.1f;
float Light::point_shaodow_far = 25.0f;
float Light::point_blur_range = 0.02f;

float Light::spot_shaodow_near = 0.1f;
float Light::spot_shaodow_far = 25.0f;
float Light::spot_blur_range = 0.02f;

float Light::area_shaodow_near = 0.1f;
float Light::area_shaodow_far = 25.0f;
float Light::area_blur_range = 0.04f;

Light::Light()
{
	assert(false && "incorrect light initialization");
}

Light::Light(LightType type, float power, glm::vec3 color)
	: light_type(type), light_power(power), light_color(color)
{
	o_type = GO_LIGHT;

	const auto [_type, _name] = ParseLightName(light_type);

	light_sprite.spr_type = _type;
	light_sprite.SetTex();

	o_name = _name + std::to_string(GetObjectID());
}

inline std::pair<SpriteType, std::string> Light::ParseLightName(LightType _type)
{
	switch (_type)
	{
	case NONELIGHT:
		return { POINT_LIGHT_SPRITE, "None Light" };
	case POINTLIGHT:
		return { POINT_LIGHT_SPRITE, "Point Light." };
	case SUNLIGHT:
		return { SUN_LIGHT_SPRITE,   "Sun." };
	case SPOTLIGHT:
		return { SPOT_LIGHT_SPRITE,  "Spot Light." };
	case AREALIGHT:
		return { POINT_LIGHT_SPRITE,  "Area Light." };
	default:
		assert(false && "Unknown Light Type");
		return { POINT_LIGHT_SPRITE, "None Light" };
	}
}

void Light::SetColor(const glm::vec3& _col)
{
	if (_col == light_color) return;

	is_light_changed = true;
	light_color = _col;
}

void Light::SetPower(float _power)
{
	if (_power == light_power) return;

	is_light_changed = true;
	light_power = std::abs(_power);
}

void Light::SetShadow(bool _state)
{
	if (_state == use_shadow) return;

	is_light_changed = true;
	use_shadow = _state;
}

void Light::SetRadius(float _rad)
{
	if (_rad == light_radius) return;

	is_light_changed = true;
	light_radius = _rad;
}

void Light::SetCutoff(float _theta)
{
	float _cutoff = glm::cos(glm::radians(_theta));
	if (_cutoff == spot_cutoff) return;

	is_light_changed = true;
	spot_cutoff = _cutoff;
}

void Light::SetOuterCutoff(float _theta)
{
	float _outer_cutoff = glm::cos(glm::radians(_theta));
	if (_outer_cutoff == spot_outer_cutoff) return;

	is_light_changed = true;
	spot_outer_cutoff = _outer_cutoff;
}

void Light::SetRatio(float _ratio)
{
	if (_ratio == area_ratio) return;

	is_light_changed = true;
	area_ratio = _ratio;
}

void Light::RenderLightSpr(const Context& ctx)
{
	light_sprite.RenderSprite(ctx, o_position, light_color, GetObjectID());
}


void* Light::GetShader()
{
	//return &_shadowmap_shader[POINTLIGHT];
	ComputeShader& shadow_shader = ComputeShader::ImportShader(
		ComputeShader::GetShadowShaderName(
			char(RenderConfigs::ShadowAlg::VSSM),
			light_type));
	return &shadow_shader;
}
