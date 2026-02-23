#include "Light.h"
#include "xdz_math.h"
#include "Input.h"
#include "shaders/ComputeShader.h"

std::array <FrameBuffer, 4> Light::_shadowmap_buffer = {};

std::array<ChainedShader, 4> Light::_shadowmap_shader = {};

std::array<glm::mat4, 6> Light::_point_6side = {
					glm::lookAt(glm::vec3(0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(-1.0,0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))};


void Light::EnableShadowMap()
{
	_shadowmap_buffer[0] = FrameBuffer(Texture(1024, 1024, Texture::DEPTH_CUBE_TEXTURE));
	_shadowmap_buffer[1] = FrameBuffer(Texture(1024, 1024, Texture::DEPTH_TEXTURE));

	_shadowmap_shader[SUNLIGHT] = ChainedShader::ImportShader("Depth_Rast.vert", "Empty.frag");

	_shadowmap_shader[POINTLIGHT] = ChainedShader::ImportShader("Empty.vert", "6sides_trans.geom", "Depth_Linear.frag");

	_shadowmap_shader[SPOTLIGHT] = ChainedShader::ImportShader("Empty.vert", "6sides_trans.geom", "Depth_Linear.frag");

	_shadowmap_shader[AREALIGHT] = ChainedShader::ImportShader("Empty_Rand.vert", "6sides_trans.geom", "Depth_Linear.frag");
}

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

	UpdateProjMatrix();
}

void Light::InitShadowMap(bool using_moment_shadow)
{
	assert(light_type != LightType::NONELIGHT);

	const Texture::TextureType flat_map = using_moment_shadow ? Texture::HDR_TEXTURE : Texture::DEPTH_TEXTURE;
	const Texture::TextureType cube_map = using_moment_shadow ? Texture::HDR_CUBE_TEXTURE : Texture::DEPTH_CUBE_TEXTURE;

	switch (light_type)
	{
	case SUNLIGHT:
		light_shadow_map = Texture(1024, 1024, flat_map);
		break;
	case POINTLIGHT:
		light_shadow_map = Texture(1024, 1024, cube_map);
		break;
	case SPOTLIGHT:
		// TODO
		break;
	case AREALIGHT:
		light_shadow_map = Texture(1024, 1024, cube_map);
		break;
	default:
		assert(false && "Unknown Light Type");
		break;
	}
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

void Light::BindShadowMapBuffer()
{
	_shadowmap_buffer[light_type].LinkTexture(light_shadow_map);
	_shadowmap_buffer[light_type].BindFrameBuffer();
}

void Light::BindShadowMapShader()
{
	_shadowmap_shader[light_type].UseShader();
	_shadowmap_shader[light_type].SetValue("shadowMatrices", 6, Light::_point_6side.data());
	const Input::RandomState random = Input::GetRandomState();
	switch (light_type)
	{
	case POINTLIGHT:
		_shadowmap_shader[light_type].SetValue("U_offset", o_position);
		_shadowmap_shader[light_type].SetValue("U_lightproj", light_proj);
		_shadowmap_shader[light_type].SetValue("far_plane", Light::point_shaodow_far);
		break;
	case SUNLIGHT:
		_shadowmap_shader[light_type].SetValue("U_lightproj", light_proj);
		break;
	case SPOTLIGHT:
		_shadowmap_shader[light_type].SetValue("U_offset", o_position);
		_shadowmap_shader[light_type].SetValue("U_lightproj", light_proj);
		_shadowmap_shader[light_type].SetValue("far_plane", Light::spot_shaodow_far);
		break;
	case AREALIGHT:
		_shadowmap_shader[light_type].SetValue("U_trans", o_Transform);
		_shadowmap_shader[light_type].SetValue("U_UV", glm::vec2(random.random_float1, random.random_float2));
		_shadowmap_shader[light_type].SetValue("ratio", area_ratio);
		_shadowmap_shader[light_type].SetValue("U_lightproj", light_proj);
		_shadowmap_shader[light_type].SetValue("far_plane", Light::area_shaodow_far);
		break;
	default:
		assert(false && "Unknown Light Type");
		break;
	}
}

void Light::BindTargetTrans(const glm::mat4& _trans)
{
	_shadowmap_shader[light_type].SetValue("U_model", _trans);
}

void Light::UpdateProjMatrix()
{
	switch (light_type)
	{
	case POINTLIGHT:
		light_proj = glm::perspective(
			glm::radians(90.0f), 
			1.0f, 
			Light::point_shaodow_near, 
			Light::point_shaodow_far
		);
		break;
	case SUNLIGHT:
		const glm::mat4 lightProjection = glm::ortho(
			-Light::sun_shaodow_field, 
			Light::sun_shaodow_field, 
			-Light::sun_shaodow_field, 
			Light::sun_shaodow_field, 
			Light::sun_shaodow_near, 
			Light::sun_shaodow_far
		);
		const glm::mat4 lightView = glm::lookAt(glm::vec3(0), glm::cross(o_dir_up, o_dir_right), glm::vec3(0, 0, 1));

		light_proj = lightProjection * lightView;
		break;
	case SPOTLIGHT:
		light_proj = glm::perspective(
			glm::radians(90.0f),
			1.0f,
			Light::spot_shaodow_near,
			Light::spot_shaodow_far
		);
		break;
	case AREALIGHT:
		light_proj = glm::perspective(
			glm::radians(90.0f),
			1.0f,
			Light::spot_shaodow_near,
			Light::spot_shaodow_far
		);
		break;
	default:
		assert(false && "Unknown Light Type");
		break;
	}
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

void Light::ConstructSAT(const RenderConfigs* config)
{
	if (!config->RequiresMomentShadow())
		return;

	auto [_1, _2, _3, gl_type] = Texture::ParseFormat(light_shadow_map.tex_type);
	const int pass_count = config->r_shadow_algorithm == RenderConfigs::ShadowAlg::VSSM ? 2 : 4;

	if (gl_type == GL_TEXTURE_2D) {
		ComputeShader& SAT = ComputeShader::ImportShader("convert/SAT");

		static Texture light_shadow_temp = Texture(light_shadow_map.GetW(), light_shadow_map.GetH(), Texture::HDR_TEXTURE);

		light_shadow_map.BindC(0, GL_READ_ONLY);
		light_shadow_temp.BindC(1, GL_WRITE_ONLY);
		SAT.RunComputeShader({ light_shadow_map.GetW(), 1 });

		light_shadow_temp.BindC(0, GL_READ_ONLY);
		light_shadow_map.BindC(1, GL_WRITE_ONLY);
		SAT.RunComputeShader({ light_shadow_map.GetH(), 1 });
	}
	else if (gl_type == GL_TEXTURE_CUBE_MAP) {
		// ComputeShader& SAT_cube = ComputeShader::ImportShader("convert/SAT_Cube");
		// Skip for now, not necessary to use SAT filtering
	}
}


