#include "Light.h"
#include "xdz_math.h"

FrameBuffer Light::_shadowmap_buffer = FrameBuffer();

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
	_shadowmap_buffer = FrameBuffer(Texture(1024, 1024, DEPTH_CUBE_TEXTURE));

	_shadowmap_shader[SUNLIGHT] = ChainedShader::ImportShader("Depth_Rast.vert", "Empty.frag");

	_shadowmap_shader[POINTLIGHT] = ChainedShader::ImportShader("Empty.vert", "6sides_trans.geom", "Depth_Linear.frag");
	_shadowmap_shader[POINTLIGHT].UseShader();
	_shadowmap_shader[POINTLIGHT].SetValue("shadowMatrices", 6, Light::_point_6side.data());

	_shadowmap_shader[SPOTLIGHT] = ChainedShader::ImportShader("Empty.vert", "6sides_trans.geom", "Depth_Linear.frag");
	_shadowmap_shader[SPOTLIGHT].UseShader();
	_shadowmap_shader[SPOTLIGHT].SetValue("shadowMatrices", 6, Light::_point_6side.data());

	_shadowmap_shader[AREALIGHT] = ChainedShader::ImportShader("Empty_Rand.vert", "6sides_trans.geom", "Depth_Linear.frag");
	_shadowmap_shader[AREALIGHT].UseShader();
	_shadowmap_shader[AREALIGHT].SetValue("shadowMatrices", 6, Light::_point_6side.data());
}

float Light::sun_shaodow_field = 20.0f;
float Light::sun_shaodow_near = -20.0f;
float Light::sun_shaodow_far = 20.0f;

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

	InitShadowMap();
	UpdateProjMatrix();
	BindShadowMapShader();
}

void Light::InitShadowMap(RenderConfigs* config/*=nullptr*/)
{
	assert(light_type != LightType::NONELIGHT);
	
	bool using_moment_shadow = false;
	if (config)
		using_moment_shadow &= config->RequiresMomentShadow();

	const TextureType flat_map = using_moment_shadow ? IBL_TEXTURE : DEPTH_TEXTURE;
	const TextureType cube_map = using_moment_shadow ? IBL_CUBE_TEXTURE : DEPTH_CUBE_TEXTURE;

	switch (light_type)
	{
	case SUNLIGHT:
		light_shadow_map = Texture(2048, 2048, flat_map);
		break;
	case POINTLIGHT:
	case SPOTLIGHT:
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

void Light::RenderLightSpr(Camera* cam)
{
	light_sprite.RenderSprite(o_position, light_color, cam);
}

void Light::BindShadowMapBuffer()
{
	_shadowmap_buffer.LinkTexture(light_shadow_map);
	_shadowmap_buffer.BindFrameBuffer();
}

void Light::BindShadowMapShader()
{

	_shadowmap_shader[light_type].UseShader();
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
		_shadowmap_shader[light_type].SetValue("U_UV", glm::vec2(EventListener::random_float1, EventListener::random_float2));
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


void Light::ConstructSAT()
{
	static ComputeShader& SAT = ComputeShader::ImportShader("convert/SAT");
	static ComputeShader& SAT_cube = ComputeShader::ImportShader("convert/SAT_Cube");
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


LightArrayBuffer::~LightArrayBuffer()
{}

void LightArrayBuffer::Init()
{
	point_buffer = StorageBuffer(CUSTOM_LIST, 0);
	sun_buffer = StorageBuffer(CUSTOM_LIST, 1);
	spot_buffer = StorageBuffer(CUSTOM_LIST, 2);
	area_buffer = StorageBuffer(CUSTOM_LIST, 3);
	poly_buffer = StorageBuffer(CUSTOM_LIST, 4);
	poly_verts_buffer = StorageBuffer(CUSTOM_LIST, 5);
	info = UniformBuffer(6);
}

void LightArrayBuffer::Bind() const
{
	point_buffer.BindBufferBase();
	sun_buffer.BindBufferBase();
	spot_buffer.BindBufferBase();
	area_buffer.BindBufferBase();
	poly_buffer.BindBufferBase();
	poly_verts_buffer.BindBufferBase();
	info.Bind(0);
	BindShadowMap();
}

LightArrayBuffer::PointStruct::PointStruct(const Light& light)
	:color(light.light_color),
	pos(light.o_position),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	radius(light.light_radius)
{
	assert(light.light_type == POINTLIGHT);
}

LightArrayBuffer::SunStruct::SunStruct(const Light& light)
	:color(light.light_color),
	pos(light.o_position),
	dir(glm::cross(light.o_dir_up, light.o_dir_right)),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	proj_trans(light.light_proj)
{
	assert(light.light_type == SUNLIGHT);
}

LightArrayBuffer::SpotStruct::SpotStruct(const Light& light)
	:color(light.light_color),
	pos(light.o_position),
	dir(-glm::cross(light.o_dir_up, light.o_dir_right)),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	cutoff(light.spot_cutoff),
	outer_cutoff(light.spot_outer_cutoff)
{
	assert(light.light_type == SPOTLIGHT);
}

LightArrayBuffer::AreaStruct::AreaStruct(const Light& light)
	:color(light.light_color),
	trans(light.o_Transform),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	ratio(light.area_ratio)
{
	assert(light.light_type == AREALIGHT);
}

void LightArrayBuffer::ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list)
{
	point_list.clear();
	sun_list.clear();
	spot_list.clear();
	area_list.clear();
	light_info_cache.clear();

	for (auto& [id, light] : light_list)
	{
		light->UpdateProjMatrix();

		shadow_cache[id] = Texture((int)cache_w, (int)cache_h, LIGHTING_CACHE);

		switch (light->light_type)
		{
		case NONELIGHT:
			break;
		case POINTLIGHT:
			light_info_cache[id] = _LightInfo(point_list.size(), light.get());
			point_list.emplace_back(*light.get());
			break;
		case SUNLIGHT:
			light_info_cache[id] = _LightInfo(sun_list.size(), light.get());
			sun_list.emplace_back(*light.get());
			break;
		case SPOTLIGHT:
			light_info_cache[id] = _LightInfo(spot_list.size(), light.get());
			spot_list.emplace_back(*light.get());
			break;
		case AREALIGHT:
			light_info_cache[id] = _LightInfo(area_list.size(), light.get());
			area_list.emplace_back(*light.get());
			break;
		default:
			assert(false && "Unknown Light Type");
			break;
		}
	}

	point_buffer.GenStorageBuffer(point_list);
	sun_buffer.GenStorageBuffer(sun_list);
	spot_buffer.GenStorageBuffer(spot_list);
	area_buffer.GenStorageBuffer(area_list);
	info.Update(GetSceneInfo());
}

void LightArrayBuffer::ParsePolygonLightData(const std::unordered_map<int, std::shared_ptr<PolygonLight>>& poly_light_list)
{
	poly_list.clear();
	poly_verts.clear();

	for (auto& al : poly_light_list)
	{
		auto& v = al.second->verts;

		poly_list.emplace_back(PolyStruct{
			al.second->light_color,

			al.second->light_power,
			(int)al.second->use_shadow,
			(int)v.size() / 2
			});

		al.second->ApplyTransform();
		for (size_t i = 0; i < v.size(); i += 2)
		{
			poly_verts.emplace_back(PolyVertStruct{
				glm::vec3(al.second->o_Transform * glm::vec4(v[i], v[i + 1], 0.0f, 1.0f))
				});
		}
	}

	poly_buffer.GenStorageBuffer(poly_list);
	poly_verts_buffer.GenStorageBuffer(poly_verts);
	info.Update(GetSceneInfo());
}

LightArrayBuffer::SceneInfo LightArrayBuffer::GetSceneInfo() const
{
	SceneInfo info{};

	info.point_count = point_list.size();
	info.sun_count = sun_list.size();
	info.spot_count = spot_list.size();
	info.area_count = area_list.size();
	info.poly_count = poly_list.size();
	info.poly_verts_count = poly_verts.size();

	return info;
}

GLsizei LightArrayBuffer::GetTotalCount() const
{
	return point_list.size() + sun_list.size() + spot_list.size() + area_list.size()/* + area_verts.size()*/;
}

GLuint LightArrayBuffer::GetSlotOffset(LightType _type) const
{
	switch (_type)
	{
	case POINTLIGHT:
		return 0;
	case SUNLIGHT:
		return GetSlotOffset(POINTLIGHT) + point_list.size();
	case SPOTLIGHT:
		return GetSlotOffset(SUNLIGHT)   + sun_list.size();
	case AREALIGHT:
		return GetSlotOffset(SPOTLIGHT)  + spot_list.size();
	default:
		assert(false && "Unknown Light Type");
		return 0;
	}
}

void LightArrayBuffer::Resize(GLuint _w, GLuint _h){
	cache_w = _w;
	cache_h = _h;

	for (auto& [_, cache] : shadow_cache)
		cache.Resize(cache_w, cache_h);
}

void LightArrayBuffer::UpdateLight(Light* light)
{
	if (light_info_cache.find(light->GetObjectID()) == light_info_cache.end())
		return;

	int loc = std::get<0>(light_info_cache[light->GetObjectID()]);
	light->UpdateProjMatrix();

	switch (light->light_type)
	{
	case POINTLIGHT:
		point_list[loc] = *light;
		point_buffer.GenStorageBuffer(point_list);
		break;
	case SUNLIGHT:
		sun_list[loc] = *light;
		sun_buffer.GenStorageBuffer(sun_list);
		break;
	case SPOTLIGHT:
		spot_list[loc] = *light;
		spot_buffer.GenStorageBuffer(spot_list);
		break;
	case AREALIGHT:
		area_list[loc] = *light;
		area_buffer.GenStorageBuffer(area_list);
		break;
	default:
		assert(false && "Unknown Light Type");
		break;
	}
}

void LightArrayBuffer::UpdateLightingCache(int frame, RenderConfigs* config)
{
	const bool is_incr_aver = config->r_sampling_average == RenderConfigs::SamplingType::IncrementAverage;

	const float point_ud_rate	= is_incr_aver ? 0.05 : 1.0 / frame;
	const float sun_ud_rate		= is_incr_aver ? 0.05 : 1.0 / frame;
	const float spot_ud_rate	= is_incr_aver ? 0.05 : 1.0 / frame;
	const float area_ud_rate	= is_incr_aver ? 0.01 : 1.0 / frame;

	const glm::vec3 random = glm::vec3(EventListener::random_float1, EventListener::random_float2, EventListener::random_float3);

	for (const auto& [id, info] : light_info_cache) {
		const auto& [loc, light] = info;
		const GLuint map_id = light->light_shadow_map.GetTexID();
		const LightType type = light->light_type;

		ComputeShader& shadow_shader = ComputeShader::ImportShader(ComputeShader::GetShadowShaderName((char)config->r_shadow_algorithm, type));

		shadow_shader.UseShader();
		shadow_shader.SetValue("offset", xdzm::map01_11(random));
		shadow_shader.SetValue("frame", frame);

		shadow_cache[id].BindC(4);
		switch (type)
		{
		case POINTLIGHT:

			Texture::BindM(map_id, 31, DEPTH_CUBE_TEXTURE);

			shadow_shader.SetValue("light_pos", light->o_position);
			shadow_shader.SetValue("light_far", Light::point_shaodow_far);
			shadow_shader.SetValue("update_rate", point_ud_rate);
			shadow_shader.SetValue("radius", light->light_radius);
			//shadow_shader.SetValue("offset", Light::point_blur_range);

			break;
		case SUNLIGHT:

			Texture::BindM(map_id, 31);
			shadow_shader.SetValue("proj_trans", light->light_proj);
			shadow_shader.SetValue("dir", sun_list[loc].dir);
			shadow_shader.SetValue("radius", Light::point_blur_range);
			shadow_shader.SetValue("update_rate", sun_ud_rate);

			break;
		case SPOTLIGHT:

			Texture::BindM(map_id, 31, DEPTH_CUBE_TEXTURE);

			shadow_shader.SetValue("light_pos", light->o_position);
			shadow_shader.SetValue("light_dir", spot_list[loc].dir);
			shadow_shader.SetValue("outer_cutoff", light->spot_outer_cutoff);
			shadow_shader.SetValue("light_far", Light::spot_shaodow_far);
			shadow_shader.SetValue("radius", Light::spot_blur_range);
			shadow_shader.SetValue("update_rate", spot_ud_rate);

			break;
		case AREALIGHT:

			Texture::BindM(map_id, 31, DEPTH_CUBE_TEXTURE);

			shadow_shader.SetValue("light_trans", light->o_Transform);
			shadow_shader.SetValue("U_UV", glm::vec2(EventListener::random_float1, EventListener::random_float2));
			shadow_shader.SetValue("ratio", light->area_ratio);
			shadow_shader.SetValue("light_far", Light::area_shaodow_far);
			shadow_shader.SetValue("radius", Light::area_blur_range);
			shadow_shader.SetValue("update_rate", area_ud_rate);

			break;
		default:
			assert(false && "Unknown Light Type");
			break;
		}

		shadow_shader.RunComputeShader(cache_w / 16, cache_h / 16);
	}
}

void LightArrayBuffer::BindShadowMap() const
{
	for (auto& [id, info] : light_info_cache) {
		const auto& [loc, light] = info;
		const LightType type = light->light_type;
		const GLuint slot = 31 - (GetSlotOffset(type) + loc);
		shadow_cache[id].Bind(slot);
	}
}

