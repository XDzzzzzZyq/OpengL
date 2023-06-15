#include "Light.h"

Light::Light()
{
	assert(false && "incorrect light initialization");
}

Light::Light(LightType type, float power, glm::vec3 color)
	: light_type(type),	light_power(power),	light_color(color)
{
	o_type = GO_LIGHT;

	const auto [_type, _name] = ParseLightName(light_type);

	light_spirit.spr_type = _type;
	light_spirit.SetTex();

	light_shadow_map = Texture(1024, 1024, IBL_CUBE_TEXTURE);

	o_name = _name + std::to_string(GetObjectID());
}

inline  std::pair<SpiritType, std::string> Light::ParseLightName(LightType _type)
{
	switch (_type)
	{
	case NONELIGHT:
		return { POINT_LIGHT_SPIRIT, "None Light"   };
	case POINTLIGHT:
		return { POINT_LIGHT_SPIRIT, "Point Light." };
	case SUNLIGHT:
		return { SUN_LIGHT_SPIRIT,   "Sun."			};
	case SPOTLIGHT:
		return { SPOT_LIGHT_SPIRIT,  "Spot Light."  };
	default:
		return { POINT_LIGHT_SPIRIT, "None Light"   };
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
	point_buffer      = StorageBuffer(CUSTOM_LIST, 0);
	sun_buffer        = StorageBuffer(CUSTOM_LIST, 1);
	spot_buffer       = StorageBuffer(CUSTOM_LIST, 2);
	area_buffer       = StorageBuffer(CUSTOM_LIST, 3);
	area_verts_buffer = StorageBuffer(CUSTOM_LIST, 4);
	info              = UniformBuffer<SceneInfo>(5);
}

void LightArrayBuffer::Bind() const
{
	point_buffer.BindBufferBase();
	sun_buffer.BindBufferBase();
	spot_buffer.BindBufferBase();
	area_buffer.BindBufferBase();
	area_verts_buffer.BindBufferBase();
	info.Bind(0);
}

LightArrayBuffer::PointStruct::PointStruct(const Light& light)
	:color(light.light_color),
	pos(light.o_position),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	radius(light.light_radius)
{}

LightArrayBuffer::SunStruct::SunStruct(const Light& light)
	:color(light.light_color),
	pos(light.o_position),
	dir(-glm::cross(light.o_dir_up, light.o_dir_right)),

	power(light.light_power),
	use_shadow((int)light.use_shadow)
{}

LightArrayBuffer::SpotStruct::SpotStruct(const Light& light)
	:color(light.light_color),
	pos(light.o_position),
	dir(-glm::cross(light.o_dir_up, light.o_dir_right)),

	power(light.light_power),
	use_shadow((int)light.use_shadow),
	cutoff(light.spot_cutoff),
	outer_cutoff(light.spot_outer_cutoff)
{}

void LightArrayBuffer::ParseLightData(const std::unordered_map<int, std::shared_ptr<Light>>& light_list)
{
	point.clear();
	sun.clear();
	spot.clear();
	id_loc_cache.clear();

	for (auto& light : light_list)
	{
		switch (light.second->light_type)
		{
		case NONELIGHT:
			break;
		case POINTLIGHT:
			id_loc_cache[light.first] = point.size();
			point.emplace_back(*light.second.get());
			break;
		case SUNLIGHT:
			id_loc_cache[light.first] = sun.size();
			sun.emplace_back(*light.second.get());
			break;
		case SPOTLIGHT:
			id_loc_cache[light.first] = spot.size();
			spot.emplace_back(*light.second.get());
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

	for (auto &al : area_light_list)
	{
		auto &v = al.second->verts;

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

	// TODO: Implement shadow map for area light
	LOOP(GetTotalCount()) {
		info.shadow_maps[i] = 32 - i - 1;          // binding from 31 -> 0
	}

	return info;
}

inline GLsizei LightArrayBuffer::GetTotalCount() const
{
	return point.size() + sun.size() + spot.size() /*+ area.size() + area_verts.size()*/;
}

void LightArrayBuffer::UpdateLight(const std::pair<int, std::shared_ptr<Light>>& light)
{
	if (id_loc_cache.find(light.first) == id_loc_cache.end())
		return;

	switch (light.second->light_type)
	{
	case NONELIGHT:
		break;
	case POINTLIGHT:
		point[id_loc_cache[light.first]] = *light.second.get();
		point_buffer.GenStorageBuffer(point);
		break;
	case SUNLIGHT:
		sun[id_loc_cache[light.first]] = *light.second.get();
		sun_buffer.GenStorageBuffer(sun);
		break;
	case SPOTLIGHT:
		spot[id_loc_cache[light.first]] = *light.second.get();
		spot_buffer.GenStorageBuffer(spot);
		break;
	default:
		break;
	}
}

