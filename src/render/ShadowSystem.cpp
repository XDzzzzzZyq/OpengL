#include "ShadowSystem.h"
#include "xdz_math.h"
#include "Input.h"
#include "shaders/ComputeShader.h"

ShadowSystem::ShadowSystem()
	:cache_w(SCREEN_W), cache_h(SCREEN_H)
{}

ShadowSystem::~ShadowSystem()
{}

void ShadowSystem::Init()
{
	point_buffer = StorageBuffer(CUSTOM_LIST, 0);
	sun_buffer = StorageBuffer(CUSTOM_LIST, 1);
	spot_buffer = StorageBuffer(CUSTOM_LIST, 2);
	area_buffer = StorageBuffer(CUSTOM_LIST, 3);
	poly_buffer = StorageBuffer(CUSTOM_LIST, 4);
	poly_verts_buffer = StorageBuffer(CUSTOM_LIST, 5);
	info = UniformBuffer(6);
}

void ShadowSystem::Bind() const
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

ShadowSystem::PointStruct::PointStruct(const Light& light)
	:color(light.light_color),
	pos(light.o_position),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	radius(light.light_radius)
{
	assert(light.light_type == POINTLIGHT);
}

ShadowSystem::SunStruct::SunStruct(const Light& light, const glm::mat4& proj)
	:color(light.light_color),
	pos(light.o_position),
	dir(glm::cross(light.o_dir_up, light.o_dir_right)),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	proj_trans(proj)
{
	assert(light.light_type == SUNLIGHT);
}

ShadowSystem::SpotStruct::SpotStruct(const Light& light)
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

ShadowSystem::AreaStruct::AreaStruct(const Light& light)
	:color(light.light_color),
	trans(light.o_Transform),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	ratio(light.area_ratio)
{
	assert(light.light_type == AREALIGHT);
}

void ShadowSystem::ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list, bool using_moment_shadow)
{
	point_list.clear();
	sun_list.clear();
	spot_list.clear();
	area_list.clear();
	light_info_cache.clear();

	for (auto& [id, light] : light_list)
	{
		UpdateProjMatrix(light.get());
		InitShadowMap(light.get(), using_moment_shadow);

		shadow_cache[id] = Texture((int)cache_w, (int)cache_h, Texture::LIGHTING_CACHE);

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
			sun_list.emplace_back(*light.get(), proj_matrices[id]);
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

void ShadowSystem::ParsePolygonLightData(const std::unordered_map<int, std::shared_ptr<PolygonLight>>& poly_light_list)
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

ShadowSystem::SceneInfo ShadowSystem::GetSceneInfo() const
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

GLsizei ShadowSystem::GetTotalCount() const
{
	return point_list.size() + sun_list.size() + spot_list.size() + area_list.size()/* + area_verts.size()*/;
}

GLuint ShadowSystem::GetSlotOffset(LightType _type) const
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
		return 0u;
	}
}

void ShadowSystem::Resize(GLuint _w, GLuint _h)
{
	cache_w = _w;
	cache_h = _h;

	for (auto& [_, cache] : shadow_cache)
		cache.Resize(cache_w, cache_h);
}

void ShadowSystem::UpdateLight(Light* light)
{
	if (light_info_cache.find(light->GetObjectID()) == light_info_cache.end()) {
		return;
	}

	int id = light->GetObjectID();
	int loc = std::get<0>(light_info_cache[id]);
	UpdateProjMatrix(light);

	switch (light->light_type)
	{
	case POINTLIGHT:
		point_list[loc] = *light;
		point_buffer.GenStorageBuffer(point_list);
		break;
	case SUNLIGHT:
		sun_list[loc] = SunStruct(*light, proj_matrices[id]);
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

void ShadowSystem::Update(int frame, RenderConfigs* config)
{
	if (!config->RequiresShadow()) {
		for (const auto& [id, _] : light_info_cache) {
			shadow_cache[id].FillColor({1,1,1,1});
		}
		return;
	}

	bool using_moment_shadow = config->RequiresMomentShadow();

	const Texture::TextureType flat_map = using_moment_shadow ? Texture::HDR_TEXTURE : Texture::DEPTH_TEXTURE;
	const Texture::TextureType cube_map = using_moment_shadow ? Texture::HDR_CUBE_TEXTURE : Texture::DEPTH_CUBE_TEXTURE;


	const bool is_incr_aver = config->r_sampling_average == RenderConfigs::SamplingType::IncrementAverage;

	const float point_ud_rate	= is_incr_aver ? 0.05f : 1.0f / frame;
	const float sun_ud_rate		= is_incr_aver ? 0.05f : 1.0f / frame;
	const float spot_ud_rate	= is_incr_aver ? 0.05f : 1.0f / frame;
	const float area_ud_rate	= is_incr_aver ? 0.01f : 1.0f / frame;

	const Input::RandomState random_state = Input::GetRandomState();
	const glm::vec3 random = glm::vec3(random_state.random_float1, random_state.random_float2, random_state.random_float3);

	for (const auto& [id, info] : light_info_cache) {
		const auto& [loc, light] = info;
		Texture& shadow_map = shadow_maps[id];
		const glm::mat4& proj = proj_matrices[id];
		const GLuint map_id = shadow_map.GetTexID();
		const LightType type = light->light_type;

		ComputeShader& shadow_shader = ComputeShader::ImportShader(ComputeShader::GetShadowShaderName(char(config->r_shadow_algorithm), type));

		shadow_shader.UseShader();
		shadow_shader.SetValue("offset", xdzm::map01_11(random));
		shadow_shader.SetValue("frame", frame);
		shadow_shader.SetValue("map_size", glm::vec2(shadow_map.GetW(), shadow_map.GetH()));

		shadow_cache[id].BindC(4);
		switch (type)
		{
		case POINTLIGHT:

			if (using_moment_shadow != prev_moment_shadow) {
				shadow_map.SaveTexture(using_moment_shadow ? "depth_vssm" : "depth_shadow", false);
				InitShadowMap(light, using_moment_shadow);
			}

			Texture::BindM(map_id, 31, cube_map);

			shadow_shader.SetValue("light_pos", light->o_position);
			shadow_shader.SetValue("light_far", Light::point_shaodow_far);
			shadow_shader.SetValue("update_rate", point_ud_rate);
			shadow_shader.SetValue("radius", light->light_radius);
			//shadow_shader.SetValue("offset", Light::point_blur_range);

			break;
		case SUNLIGHT:

			Texture::BindM(map_id, 31, flat_map);

			shadow_shader.SetValue("proj_trans", proj);
			shadow_shader.SetValue("dir", sun_list[loc].dir);
			shadow_shader.SetValue("radius", Light::point_blur_range);
			shadow_shader.SetValue("light_size", Light::sun_shaodow_field);
			shadow_shader.SetValue("update_rate", sun_ud_rate);

			if (using_moment_shadow != prev_moment_shadow) {
				shadow_map.SaveTexture(using_moment_shadow ? "depth_sun_vssm" : "depth_sun_shadow", false);
				InitShadowMap(light, using_moment_shadow);
			}

			break;
		case SPOTLIGHT:

			Texture::BindM(map_id, 31, cube_map);

			shadow_shader.SetValue("light_pos", light->o_position);
			shadow_shader.SetValue("light_dir", spot_list[loc].dir);
			shadow_shader.SetValue("outer_cutoff", light->spot_outer_cutoff);
			shadow_shader.SetValue("light_far", Light::spot_shaodow_far);
			shadow_shader.SetValue("radius", Light::spot_blur_range);
			shadow_shader.SetValue("update_rate", spot_ud_rate);

			break;
		case AREALIGHT:

			Texture::BindM(map_id, 31, cube_map);

			shadow_shader.SetValue("light_trans", light->o_Transform);
			shadow_shader.SetValue("U_UV", glm::vec2(random_state.random_float1, random_state.random_float2));
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

	prev_moment_shadow = using_moment_shadow;
}

void ShadowSystem::BindShadowMap() const
{
	for (auto& [id, info] : light_info_cache) {
		const auto& [loc, light] = info;
		const LightType type = light->light_type;
		const GLuint slot = 31 - (GetSlotOffset(type) + loc);
		shadow_cache[id].Bind(slot);
	}
}

void ShadowSystem::InitShadowMap(Light* light, bool using_moment_shadow)
{
	assert(light->light_type != LightType::NONELIGHT);

	const int id = light->GetObjectID();
	const Texture::TextureType flat_map = using_moment_shadow ? Texture::HDR_TEXTURE : Texture::DEPTH_TEXTURE;
	const Texture::TextureType cube_map = using_moment_shadow ? Texture::HDR_CUBE_TEXTURE : Texture::DEPTH_CUBE_TEXTURE;

	switch (light->light_type)
	{
	case SUNLIGHT:
		shadow_maps[id] = Texture(1024, 1024, flat_map);
		break;
	case POINTLIGHT:
		shadow_maps[id] = Texture(1024, 1024, cube_map);
		break;
	case SPOTLIGHT:
		// TODO
		break;
	case AREALIGHT:
		shadow_maps[id] = Texture(1024, 1024, cube_map);
		break;
	default:
		assert(false && "Unknown Light Type");
		break;
	}
}

void ShadowSystem::UpdateProjMatrix(Light* light)
{
	const int id = light->GetObjectID();

	switch (light->light_type)
	{
	case POINTLIGHT:
		proj_matrices[id] = glm::perspective(
			glm::radians(90.0f),
			1.0f,
			Light::point_shaodow_near,
			Light::point_shaodow_far
		);
		break;
	case SUNLIGHT:
	{
		const glm::mat4 lightProjection = glm::ortho(
			-Light::sun_shaodow_field,
			Light::sun_shaodow_field,
			-Light::sun_shaodow_field,
			Light::sun_shaodow_field,
			Light::sun_shaodow_near,
			Light::sun_shaodow_far
		);
		const glm::mat4 lightView = glm::lookAt(glm::vec3(0), glm::cross(light->o_dir_up, light->o_dir_right), glm::vec3(0, 0, 1));
		proj_matrices[id] = lightProjection * lightView;
		break;
	}
	case SPOTLIGHT:
		proj_matrices[id] = glm::perspective(
			glm::radians(90.0f),
			1.0f,
			Light::spot_shaodow_near,
			Light::spot_shaodow_far
		);
		break;
	case AREALIGHT:
		proj_matrices[id] = glm::perspective(
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

void ShadowSystem::ConstructSAT(Light* light, const RenderConfigs* config)
{
	if (!config->RequiresMomentShadow())
		return;

	const int id = light->GetObjectID();
	Texture& shadow_map = shadow_maps[id];

	auto [_1, _2, _3, gl_type] = Texture::ParseFormat(shadow_map.tex_type);
	const int pass_count = config->r_shadow_algorithm == RenderConfigs::ShadowAlg::VSSM ? 2 : 4;

	if (gl_type == GL_TEXTURE_2D) {
		ComputeShader& SAT = ComputeShader::ImportShader("convert/SAT");

		static Texture light_shadow_temp = Texture(shadow_map.GetW(), shadow_map.GetH(), Texture::HDR_TEXTURE);

		shadow_map.BindC(0, GL_READ_ONLY);
		light_shadow_temp.BindC(1, GL_WRITE_ONLY);
		SAT.RunComputeShader({ shadow_map.GetW(), 1 });

		light_shadow_temp.BindC(0, GL_READ_ONLY);
		shadow_map.BindC(1, GL_WRITE_ONLY);
		SAT.RunComputeShader({ shadow_map.GetH(), 1 });
	}
	else if (gl_type == GL_TEXTURE_CUBE_MAP) {
		// ComputeShader& SAT_cube = ComputeShader::ImportShader("convert/SAT_Cube");
		// Skip for now, not necessary to use SAT filtering
	}
}

std::array<FrameBuffer, 4> ShadowSystem::_shadowmap_buffer = {};

std::array<ChainedShader, 4> ShadowSystem::_shadowmap_shader = {};

std::array<glm::mat4, 6> ShadowSystem::_point_6side = {
				glm::lookAt(glm::vec3(0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
				glm::lookAt(glm::vec3(0), glm::vec3(-1.0,0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
				glm::lookAt(glm::vec3(0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
				glm::lookAt(glm::vec3(0), glm::vec3(0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
				glm::lookAt(glm::vec3(0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
				glm::lookAt(glm::vec3(0), glm::vec3(0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))};

void ShadowSystem::EnableShadowMap()
{
	_shadowmap_buffer[0] = FrameBuffer(Texture(1024, 1024, Texture::DEPTH_CUBE_TEXTURE));
	_shadowmap_buffer[1] = FrameBuffer(Texture(1024, 1024, Texture::DEPTH_TEXTURE));

	_shadowmap_shader[SUNLIGHT]   = ChainedShader::ImportShader("Depth_Rast.vert", "Empty.frag");
	_shadowmap_shader[POINTLIGHT] = ChainedShader::ImportShader("Empty.vert", "6sides_trans.geom", "Depth_Linear.frag");
	_shadowmap_shader[SPOTLIGHT]  = ChainedShader::ImportShader("Empty.vert", "6sides_trans.geom", "Depth_Linear.frag");
	_shadowmap_shader[AREALIGHT]  = ChainedShader::ImportShader("Empty_Rand.vert", "6sides_trans.geom", "Depth_Linear.frag");
}

void ShadowSystem::BindShadowMapBuffer(Light* light, Texture& shadow_map)
{
	_shadowmap_buffer[light->light_type].LinkTexture(shadow_map);
	_shadowmap_buffer[light->light_type].BindFrameBuffer();
}

void ShadowSystem::BindShadowMapShader(Light* light, const glm::mat4& proj)
{
	const LightType type = light->light_type;
	_shadowmap_shader[type].UseShader();
	const Input::RandomState random = Input::GetRandomState();
	switch (type)
	{
	case POINTLIGHT:
		_shadowmap_shader[type].SetValue("shadowMatrices", 6, _point_6side.data());
		_shadowmap_shader[type].SetValue("U_offset", light->o_position);
		_shadowmap_shader[type].SetValue("U_lightproj", proj);
		_shadowmap_shader[type].SetValue("far_plane", Light::point_shaodow_far);
		break;
	case SUNLIGHT:
		_shadowmap_shader[type].SetValue("U_lightproj", proj);
		break;
	case SPOTLIGHT:
		_shadowmap_shader[type].SetValue("shadowMatrices", 6, _point_6side.data());
		_shadowmap_shader[type].SetValue("U_offset", light->o_position);
		_shadowmap_shader[type].SetValue("U_lightproj", proj);
		_shadowmap_shader[type].SetValue("far_plane", Light::spot_shaodow_far);
		break;
	case AREALIGHT:
		_shadowmap_shader[type].SetValue("shadowMatrices", 6, _point_6side.data());
		_shadowmap_shader[type].SetValue("U_trans", light->o_Transform);
		_shadowmap_shader[type].SetValue("U_UV", glm::vec2(random.random_float1, random.random_float2));
		_shadowmap_shader[type].SetValue("ratio", light->area_ratio);
		_shadowmap_shader[type].SetValue("U_lightproj", proj);
		_shadowmap_shader[type].SetValue("far_plane", Light::area_shaodow_far);
		break;
	default:
		assert(false && "Unknown Light Type");
		break;
	}
}

void ShadowSystem::BindTargetTrans(Light* light, const glm::mat4& _trans)
{
	_shadowmap_shader[light->light_type].SetValue("U_model", _trans);
}
