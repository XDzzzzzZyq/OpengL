#include "Light.h"

FrameBuffer Light::_shadowmap_buffer = FrameBuffer();

FastLoadShader Light::_shadowmap_shader = FastLoadShader();

ChainedShader Light::_pointshadow_shader = ChainedShader();

std::array<glm::mat4, 6> Light::_point_6side = {
					glm::lookAt(glm::vec3(0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(-1.0,0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
					glm::lookAt(glm::vec3(0), glm::vec3(0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)) };


float Light::sun_shaodow_field = 20.0f;
float Light::sun_shaodow_near = -20.0f;
float Light::sun_shaodow_far = 20.0f;
float Light::point_shaodow_near = 0.1f;
float Light::point_shaodow_far = 25.0f;

Light::Light()
{
	assert(false && "incorrect light initialization");
}

Light::Light(LightType type, float power, glm::vec3 color)
	: light_type(type), light_power(power), light_color(color)
{
	o_type = GO_LIGHT;

	const auto [_type, _name] = ParseLightName(light_type);

	light_spirit.spr_type = _type;
	light_spirit.SetTex();

	o_name = _name + std::to_string(GetObjectID());

	InitShadowMap();
	UpdateProjMatrix();
	BindShadowMapShader();
}

void Light::InitShadowMap()
{
	assert(light_type != LightType::NONELIGHT);

	switch (light_type)
	{
	case POINTLIGHT:
		light_shadow_map = Texture(1024, 1024, DEPTH_CUBE_TEXTURE);
		break;
	case SUNLIGHT:
		light_shadow_map = Texture(2048, 2048, DEPTH_TEXTURE);
		break;
	case SPOTLIGHT:
		light_shadow_map = Texture(1024, 1024, DEPTH_CUBE_TEXTURE);
		break;
	default:
		assert(false);
	}
	
	if (_shadowmap_buffer.GetFrameBufferID() == 0)
		_shadowmap_buffer = FrameBuffer(light_shadow_map);

	if (_shadowmap_shader.getProgramID() == 0)
		_shadowmap_shader = FastLoadShader("Depth_Rast", "Empty");

	if (_pointshadow_shader.getProgramID() == 0) {
		_pointshadow_shader = ChainedShader({ "Empty.vert", "6sides_trans.geom", "Depth_Linear.frag"});
		_pointshadow_shader.UseShader();
		_pointshadow_shader.SetValue("shadowMatrices", 6, Light::_point_6side.data());

		for(auto& m : Light::_point_6side)
			DEBUG(m)
	}
}

inline std::pair<SpiritType, std::string> Light::ParseLightName(LightType _type)
{
	switch (_type)
	{
	case NONELIGHT:
		return { POINT_LIGHT_SPIRIT, "None Light" };
	case POINTLIGHT:
		return { POINT_LIGHT_SPIRIT, "Point Light." };
	case SUNLIGHT:
		return { SUN_LIGHT_SPIRIT,   "Sun." };
	case SPOTLIGHT:
		return { SPOT_LIGHT_SPIRIT,  "Spot Light." };
	default:
		return { POINT_LIGHT_SPIRIT, "None Light" };
	}
}

void Light::SetColor(ImVec4 _col)
{
	if (ImVec4_vec3(_col) == light_color) return;

	is_light_changed = true;
	light_color = ImVec4_vec3(_col);
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

void Light::RenderLightSpr(Camera* cam)
{
	light_spirit.RenderSpirit(vec3_stdVec6(o_position, light_color), cam);
}

void Light::BindShadowMapBuffer()
{
	_shadowmap_buffer.LinkTexture(light_shadow_map);
	_shadowmap_buffer.BindFrameBuffer();
}

void Light::BindShadowMapShader()
{
	switch (light_type)
	{
	case POINTLIGHT:
		_pointshadow_shader.UseShader();
		_pointshadow_shader.SetValue("U_offset", o_position);
		_pointshadow_shader.SetValue("U_lightproj", light_proj);
		_pointshadow_shader.SetValue("far_plane", Light::point_shaodow_far);
		break;
	case SUNLIGHT:
		_shadowmap_shader.UseShader();
		_shadowmap_shader.SetValue("U_lightproj", light_proj);
		break;
	case SPOTLIGHT:
		break;
	}
}

void Light::BindTargetTrans(const glm::mat4& _trans)
{
	switch (light_type)
	{
	case POINTLIGHT:
		_pointshadow_shader.SetValue("U_model", _trans);
		break;
	case SUNLIGHT:
		_shadowmap_shader.SetValue("U_model", _trans);
		break;
	case SPOTLIGHT:
		break;
	}
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
		const glm::mat4 lightView = glm::lookAt(glm::cross(o_dir_up, o_dir_right), glm::vec3(0), glm::vec3(0, 0, 1));

		light_proj = lightProjection * lightView;
		break;
	case SPOTLIGHT:
		break;
	}
}





/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


LightArrayBuffer::~LightArrayBuffer()
{
	point_buffer.DeleteBuffer();
	sun_buffer.DeleteBuffer();
	spot_buffer.DeleteBuffer();
	area_buffer.DeleteBuffer();
	area_verts_buffer.DeleteBuffer();
	info.DeleteBuffer();
}

void LightArrayBuffer::Init()
{
	point_buffer = StorageBuffer(CUSTOM_LIST, 0);
	sun_buffer = StorageBuffer(CUSTOM_LIST, 1);
	spot_buffer = StorageBuffer(CUSTOM_LIST, 2);
	area_buffer = StorageBuffer(CUSTOM_LIST, 3);
	area_verts_buffer = StorageBuffer(CUSTOM_LIST, 4);
	info = UniformBuffer<SceneInfo>(5);
}

void LightArrayBuffer::Bind() const
{
	point_buffer.BindBufferBase();
	sun_buffer.BindBufferBase();
	spot_buffer.BindBufferBase();
	area_buffer.BindBufferBase();
	area_verts_buffer.BindBufferBase();
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
	dir(-glm::cross(light.o_dir_up, light.o_dir_right)),

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

void LightArrayBuffer::ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list)
{
	point.clear();
	sun.clear();
	spot.clear();
	light_info_cache.clear();

	for (auto& [id, light] : light_list)
	{
		GLuint map_id = light->light_shadow_map.GetTexID();
		light->UpdateProjMatrix();

		switch (light->light_type)
		{
		case NONELIGHT:
			break;
		case POINTLIGHT:
			light_info_cache[id] = LightInfo(point.size(), POINTLIGHT, map_id);
			point.emplace_back(*light.get());
			break;
		case SUNLIGHT:
			light_info_cache[id] = LightInfo(sun.size(), SUNLIGHT, map_id);
			sun.emplace_back(*light.get());
			break;
		case SPOTLIGHT:
			light_info_cache[id] = LightInfo(spot.size(), SPOTLIGHT, map_id);
			spot.emplace_back(*light.get());
			break;
		default:
			break;
		}
	}

	point_buffer.GenStorageBuffer(point);
	sun_buffer.GenStorageBuffer(sun);
	spot_buffer.GenStorageBuffer(spot);
	info.Update(GetSceneInfo());
}

void LightArrayBuffer::ParseAreaLightData(const std::unordered_map<int, std::shared_ptr<AreaLight>>& area_light_list)
{
	area.clear();
	area_verts.clear();

	for (auto& al : area_light_list)
	{
		auto& v = al.second->verts;

		area.emplace_back(AreaStruct{
			al.second->light_color,

			al.second->light_power,
			(int)al.second->use_shadow,
			(int)v.size() / 2
			});

		al.second->ApplyTransform();
		for (size_t i = 0; i < v.size(); i += 2)
		{
			area_verts.emplace_back(AreaVertStruct{
				glm::vec3(al.second->o_Transform * glm::vec4(v[i], v[i + 1], 0.0f, 1.0f))
				});
		}
	}

	area_buffer.GenStorageBuffer(area);
	area_verts_buffer.GenStorageBuffer(area_verts);
	info.Update(GetSceneInfo());
}

LightArrayBuffer::SceneInfo LightArrayBuffer::GetSceneInfo() const
{
	SceneInfo info{};

	info.point_count = point.size();
	info.sun_count = sun.size();
	info.spot_count = spot.size();
	info.area_count = area.size();
	info.area_verts_count = area_verts.size();

	return info;
}

inline GLsizei LightArrayBuffer::GetTotalCount() const
{
	return point.size() + sun.size() + spot.size() /*+ area.size() + area_verts.size()*/;
}

GLuint LightArrayBuffer::GetSlotOffset(LightType _type) const
{
	switch (_type)
	{
	case POINTLIGHT:
		return 0;
	case SUNLIGHT:
		return point.size();
	case SPOTLIGHT:
		return point.size() + sun.size();
	default:
		return 0;
	}
}

void LightArrayBuffer::UpdateLight(const std::pair<int, std::shared_ptr<Light>>& light)
{
	if (light_info_cache.find(light.first) == light_info_cache.end())
		return;

	int loc = std::get<0>(light_info_cache[light.first]);
	light.second->UpdateProjMatrix();

	switch (light.second->light_type)
	{
	case NONELIGHT:
		break;
	case POINTLIGHT:
		point[loc] = *light.second.get();
		point_buffer.GenStorageBuffer(point);
		break;
	case SUNLIGHT:
		sun[loc] = *light.second.get();
		sun_buffer.GenStorageBuffer(sun);
		break;
	case SPOTLIGHT:
		spot[loc] = *light.second.get();
		spot_buffer.GenStorageBuffer(spot);
		break;
	default:
		break;
	}
}

void LightArrayBuffer::BindShadowMap() const
{
	for (auto& [id, info] : light_info_cache) {
		LightType type = std::get<1>(info);
		GLuint slot = 31 - (GetSlotOffset(type) + std::get<0>(info));
		Texture::BindM(std::get<2>(info), slot, type == SUNLIGHT ? DEPTH_TEXTURE : DEPTH_CUBE_TEXTURE);
	}
}

